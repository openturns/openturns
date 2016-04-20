//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for Continuous distributions
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <cmath>
#include <cstdlib>

#include "openturns/ContinuousDistribution.hxx"
#include "openturns/Collection.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/MethodBoundNumericalMathEvaluationImplementation.hxx"
#include "openturns/GaussKronrod.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ContinuousDistribution);

/* Default constructor */
ContinuousDistribution::ContinuousDistribution()
  : DistributionImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
ContinuousDistribution * ContinuousDistribution::clone() const
{
  return new ContinuousDistribution(*this);
}

/* Comparison operator */
Bool ContinuousDistribution::operator ==(const ContinuousDistribution & other) const
{
  return (this == &other);
}

/* String converter */
String ContinuousDistribution::__repr__() const
{
  OSS oss;
  oss << "class=" << ContinuousDistribution::GetClassName();
  return oss;
}

/* Get the DDF of the distributionImplementation */
NumericalPoint ContinuousDistribution::computeDDF(const NumericalPoint & point) const
{
  const UnsignedInteger dimension = getDimension();
  NumericalPoint ddf(dimension);
  const NumericalScalar h = std::pow(pdfEpsilon_, 1.0 / 3.0);
  LOGINFO(OSS() << "h=" << h);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    NumericalPoint left(point);
    left[i] += h;
    NumericalPoint right(point);
    right[i] -= h;
    const NumericalScalar denom = left[i] - right[i];
    const NumericalScalar pdfLeft = computePDF(left);
    const NumericalScalar pdfRight = computePDF(right);
    ddf[i] = (pdfLeft - pdfRight) / denom;
    LOGINFO(OSS() << "left=" << left << ", right=" << right << ", pdfLeft=" << pdfLeft << ", pdfRight=" << pdfRight);
  }
  return ddf;
}

/* Get the PDF of the distribution */
NumericalScalar ContinuousDistribution::computePDF(const NumericalPoint & point) const
{
  throw NotYetImplementedException(HERE) << "In ContinuousDistribution::computePDF(const NumericalPoint & point) const";
}

/* Get the CDF of the distribution */
NumericalScalar ContinuousDistribution::computeCDF(const NumericalPoint & point) const
{
  const UnsignedInteger dimension = getDimension();
  const NumericalPoint lowerBounds(getRange().getLowerBound());
  const NumericalPoint upperBounds(getRange().getUpperBound());
  // Indices of the components to take into account in the computation
  Indices toKeep(0);
  NumericalPoint reducedPoint(0);
  for (UnsignedInteger k = 0; k < dimension; ++ k)
  {
    const NumericalScalar xK = point[k];
    // Early exit if one component is less than its corresponding range lower bound
    if (xK <= lowerBounds[k]) return 0.0;
    // Keep only the indices for which xK is less than its corresponding range upper bound
    // Marginalize the others
    if (xK < upperBounds[k])
    {
      toKeep.add(k);
      reducedPoint.add(xK);
    }
  } // k
  // The point has all its components greater than the corresponding range upper bound
  if (toKeep.getSize() == 0)
    {
      return 1.0;
    }
  // The point has some components greater than the corresponding range upper bound
  if (toKeep.getSize() != dimension)
  {
    // Try to reduce the dimension
    try
    {
      return getMarginal(toKeep)->computeCDF(reducedPoint);
    }
    catch (...)
    {
      // Fallback on the default algorithm if the getMarginal() method is not implemented
    }
  }
  const Interval interval(getRange().getLowerBound(), point);
  LOGINFO(OSS() << "In ContinuousDistribution::computeCDF, using computeProbabilityContinuous(), interval=" << interval.__str__());
  return computeProbabilityContinuous(interval);
}

/* Get the survival function of the distribution */
NumericalScalar ContinuousDistribution::computeSurvivalFunction(const NumericalPoint & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (dimension == 1) return computeComplementaryCDF(point);
  const NumericalPoint lowerBounds(getRange().getLowerBound());
  const NumericalPoint upperBounds(getRange().getUpperBound());
  Bool allOutside = true;
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    if (point[i] >= upperBounds[i]) return 0.0;
    allOutside &= (point[i] <= lowerBounds[i]);
  }
  if (allOutside) return 1.0;
  const NumericalScalar survival = computeProbability(Interval(point, getRange().getUpperBound()));
  return survival;
}

/* Build a C1 interpolation of the CDF function for 1D continuous distributions */
Collection<PiecewiseHermiteEvaluationImplementation> ContinuousDistribution::interpolateCDF(const UnsignedInteger n)
{
  if (getDimension() != 1) throw NotYetImplementedException(HERE) << "In ContinuousDistribution::interpolateCDF(const UnsignedInteger n): cannot interpolate CDF for multidimensional distributions.";
  const PDFWrapper pdfWrapper(this);
  const NumericalMathFunction fPDF(bindMethod<PDFWrapper, NumericalPoint, NumericalPoint>(pdfWrapper, &PDFWrapper::computePDF, 1, 1));
  const NumericalScalar xMin = getRange().getLowerBound()[0];
  const NumericalScalar xMax = getRange().getUpperBound()[0];
  const NumericalScalar mu = getMean()[0];
  NumericalPoint locationsCDF(n);
  NumericalPoint locationsCCDF(n);
  NumericalPoint valuesCDF(n);
  NumericalPoint valuesCCDF(n);
  NumericalPoint derivativesCDF(n);
  NumericalPoint derivativesCCDF(n);
  NumericalScalar xCDFOld = xMin;
  NumericalScalar xCCDFOld = xMax;
  locationsCDF[0] = xMin;
  locationsCCDF[n - 1] = xMax;
  GaussKronrod algo;
  const NumericalScalar stepCDF = (mu - xMin) / (n - 1.0);
  const NumericalScalar stepCCDF = (xMax - mu) / (n - 1.0);
  for (UnsignedInteger i = 1; i < n; ++i)
  {
    const NumericalScalar xCDF = xMin + i * stepCDF;
    const NumericalScalar xCCDF = xMax - i * stepCCDF;
    locationsCDF[i] = xCDF;
    locationsCCDF[n - i - 1] = xCCDF;
    NumericalPoint ai;
    NumericalPoint bi;
    NumericalSample fi;
    NumericalPoint ei;
    NumericalScalar error;
    valuesCDF[i] = valuesCDF[i - 1] + algo.integrate(fPDF, xCDFOld, xCDF, error, ai, bi, fi, ei)[0];
    valuesCCDF[n - i - 1] = valuesCCDF[n - i] + algo.integrate(fPDF, xCCDF, xCCDFOld, error, ai, bi, fi, ei)[0];
    derivativesCDF[i] = computePDF(xCDF);
    derivativesCCDF[n - i - 1] = -computePDF(xCCDF);
    xCDFOld = xCDF;
    xCCDFOld = xCCDF;
  }
  Collection<PiecewiseHermiteEvaluationImplementation> coll(2);
  coll[0] = PiecewiseHermiteEvaluationImplementation(locationsCDF, valuesCDF, derivativesCDF);
  coll[1] = PiecewiseHermiteEvaluationImplementation(locationsCCDF, valuesCCDF, derivativesCCDF);
  return coll;
}

/* Tell if the distribution is continuous */
Bool ContinuousDistribution::isContinuous() const
{
  return true;
}

END_NAMESPACE_OPENTURNS

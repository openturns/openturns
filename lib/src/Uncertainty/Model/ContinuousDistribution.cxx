//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for Continuous distributions
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <cmath>
#include <cstdlib>

#include "openturns/ContinuousDistribution.hxx"
#include "openturns/Collection.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/MethodBoundEvaluation.hxx"
#include "openturns/GaussKronrod.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ContinuousDistribution)

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

/* Get the PDF of the distribution */
Scalar ContinuousDistribution::computePDF(const Point & ) const
{
  throw NotYetImplementedException(HERE) << "In ContinuousDistribution::computePDF(const Point & point) const";
}

/* Get the CDF of the distribution */
Scalar ContinuousDistribution::computeCDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  const Point lowerBounds(getRange().getLowerBound());
  const Point upperBounds(getRange().getUpperBound());
  // Indices of the components to take into account in the computation
  Indices toKeep(0);
  Point reducedPoint(0);
  for (UnsignedInteger k = 0; k < dimension; ++ k)
  {
    const Scalar xK = point[k];
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
      return getMarginal(toKeep).computeCDF(reducedPoint);
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
Scalar ContinuousDistribution::computeSurvivalFunction(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (dimension == 1) return computeComplementaryCDF(point);
  const Point lowerBounds(getRange().getLowerBound());
  const Point upperBounds(getRange().getUpperBound());
  Bool allOutside = true;
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    if (point[i] >= upperBounds[i]) return 0.0;
    allOutside &= (point[i] <= lowerBounds[i]);
  }
  if (allOutside) return 1.0;
  const Scalar survival = computeProbability(Interval(point, getRange().getUpperBound()));
  return survival;
}

/* Build a C1 interpolation of the CDF function for 1D continuous distributions */
Collection<PiecewiseHermiteEvaluation> ContinuousDistribution::interpolateCDF(const UnsignedInteger n)
{
  if (getDimension() != 1) throw NotYetImplementedException(HERE) << "In ContinuousDistribution::interpolateCDF(const UnsignedInteger n): cannot interpolate CDF for multidimensional distributions.";
  const PDFWrapper pdfWrapper(this);
  const Scalar xMin = getRange().getLowerBound()[0];
  const Scalar xMax = getRange().getUpperBound()[0];
  const Scalar mu = getMean()[0];
  Point locationsCDF(n);
  Point locationsCCDF(n);
  Point valuesCDF(n);
  Point valuesCCDF(n);
  Point derivativesCDF(n);
  Point derivativesCCDF(n);
  Scalar xCDFOld = xMin;
  Scalar xCCDFOld = xMax;
  locationsCDF[0] = xMin;
  locationsCCDF[n - 1] = xMax;
  GaussKronrod algo;
  const Scalar stepCDF = (mu - xMin) / (n - 1.0);
  const Scalar stepCCDF = (xMax - mu) / (n - 1.0);
  for (UnsignedInteger i = 1; i < n; ++i)
  {
    const Scalar xCDF = xMin + i * stepCDF;
    const Scalar xCCDF = xMax - i * stepCCDF;
    locationsCDF[i] = xCDF;
    locationsCCDF[n - i - 1] = xCCDF;
    Point ai;
    Point bi;
    Sample fi;
    Point ei;
    Scalar error = -1.0;
    valuesCDF[i] = valuesCDF[i - 1] + algo.integrate(pdfWrapper, xCDFOld, xCDF, error, ai, bi, fi, ei)[0];
    valuesCCDF[n - i - 1] = valuesCCDF[n - i] + algo.integrate(pdfWrapper, xCCDF, xCCDFOld, error, ai, bi, fi, ei)[0];
    derivativesCDF[i] = computePDF(xCDF);
    derivativesCCDF[n - i - 1] = -computePDF(xCCDF);
    xCDFOld = xCDF;
    xCCDFOld = xCCDF;
  }
  Collection<PiecewiseHermiteEvaluation> coll(2);
  coll[0] = PiecewiseHermiteEvaluation(locationsCDF, valuesCDF, derivativesCDF);
  coll[1] = PiecewiseHermiteEvaluation(locationsCCDF, valuesCCDF, derivativesCCDF);
  return coll;
}

/* Tell if the distribution is continuous */
Bool ContinuousDistribution::isContinuous() const
{
  return true;
}

END_NAMESPACE_OPENTURNS

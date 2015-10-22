//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all ComposedDistributions
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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

#include "ComposedDistribution.hxx"
#include "MarginalTransformationEvaluation.hxx"
#include "MarginalTransformationGradient.hxx"
#include "MarginalTransformationHessian.hxx"
#include "NatafEllipticalDistributionEvaluation.hxx"
#include "NatafEllipticalDistributionGradient.hxx"
#include "NatafEllipticalDistributionHessian.hxx"
#include "InverseNatafEllipticalDistributionEvaluation.hxx"
#include "InverseNatafEllipticalDistributionGradient.hxx"
#include "InverseNatafEllipticalDistributionHessian.hxx"
#include "LinearNumericalMathFunction.hxx"
#include "Indices.hxx"
#include "PersistentObjectFactory.hxx"
#include "Uniform.hxx"
#include "Normal.hxx"
#include "IndependentCopula.hxx"
#include "NormalCopula.hxx"
#include "Log.hxx"
#include "TBB.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ComposedDistribution);

static const Factory<ComposedDistribution> RegisteredFactory;

/* Default constructor */
ComposedDistribution::ComposedDistribution()
  : DistributionImplementation()
  , distributionCollection_()
  , copula_(IndependentCopula(1))
{
  setName("ComposedDistribution");
  setDimension(1);
  DistributionCollection coll(1);
  coll[0] = Uniform();
  // This call set also the range.
  setDistributionCollection( coll );
}

/* Default constructor */
ComposedDistribution::ComposedDistribution(const DistributionCollection & coll)
  : DistributionImplementation()
  , distributionCollection_()
  , copula_(IndependentCopula(coll.getSize()))
{
  setName("ComposedDistribution");
  setDimension(coll.getSize());
  // We can NOT set distributionCollection_ in the member area of the constructor
  // because we must check before if the collection is valid (ie, if all the
  // distributions of the collection have the dimension 1). We do this by calling
  // the setDistributionCollection() method that do it for us.
  // This call set also the range.
  setDistributionCollection( coll );
}

/* Default constructor */
ComposedDistribution::ComposedDistribution(const DistributionCollection & coll,
    const Distribution & copula)
  : DistributionImplementation()
  , distributionCollection_()
  , copula_(copula)
{
  setName("ComposedDistribution");
  if (!copula.isCopula()) throw InvalidArgumentException(HERE) << "Error: the given distribution=" << copula << " is not a copula.";
  setDimension(copula.getDimension());
  // We can NOT set distributionCollection_ in the member area of the constructor
  // because we must check before if the collection is valid (ie, if all the
  // distributions of the collection have the dimension 1). We do this by calling
  // the setDistributionCollection() method that do it for us.
  // This call set also the range.
  setDistributionCollection( coll );
}

/* Comparison operator */
Bool ComposedDistribution::operator ==(const ComposedDistribution & other) const
{
  if (this == &other) return true;
  return (copula_ == other.copula_) && (distributionCollection_ == other.distributionCollection_);
}

/* String converter */
String ComposedDistribution::__repr__() const
{
  OSS oss;
  oss << "class=" << ComposedDistribution::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " copula=" << copula_;
  for (UnsignedInteger i = 0; i < getDimension(); ++i) oss << " marginal[" << i << "]=" << distributionCollection_[i];
  return oss;
}

String ComposedDistribution::__str__(const String & offset) const
{
  OSS oss;
  oss << offset << getClassName() << "(";
  String separator("");
  for (UnsignedInteger i = 0; i < distributionCollection_.getSize(); ++i)
  {
    oss << separator << distributionCollection_[i].__str__();
    separator = ", ";
  }
  if (getDimension() > 1) oss << ", " << copula_.__str__();
  oss << ")";
  return oss;
}


/* Distribution collection accessor */
void ComposedDistribution::setDistributionCollection(const DistributionCollection & coll)
{
  // Check if the collection is not empty
  const UnsignedInteger size(coll.getSize());
  if ((getDimension() != 0) && (size != getDimension())) throw InvalidArgumentException(HERE) << "The distribution collection must have a size equal to the distribution dimension";
  Description description(size);
  NumericalPoint lowerBound(size);
  NumericalPoint upperBound(size);
  Interval::BoolCollection finiteLowerBound(size);
  Interval::BoolCollection finiteUpperBound(size);
  if (size == 0) throw InvalidArgumentException(HERE) << "Collection of distributions is empty";
  // First, check that all the marginal distributions are of dimension 1
  Bool parallel = copula_.getImplementation()->isParallel();
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    if (coll[i].getDimension() != 1) throw InvalidArgumentException(HERE) << "The marginal distribution " << i << " is of dimension " << coll[i].getDimension() << ", which is different from 1.";
    parallel = parallel && coll[i].getImplementation()->isParallel();
    const Interval marginalRange(coll[i].getRange());
    lowerBound[i] = marginalRange.getLowerBound()[0];
    upperBound[i] = marginalRange.getUpperBound()[0];
    finiteLowerBound[i] = marginalRange.getFiniteLowerBound()[0];
    finiteUpperBound[i] = marginalRange.getFiniteUpperBound()[0];
    // The description of the ComposedDistribution is built first by using the marginal description
    // then by using the marginal name if the description is empty, which should never occur
    const String marginalDescription(coll[i].getDescription()[0]);
    if (marginalDescription.size() > 0) description[i] = marginalDescription;
    else
    {
      LOGINFO(OSS() << "Warning: using the name of the marginal " << i << " instead of its description for building the description of the ComposedDistribution, because the marginal description is empty.");
      const String marginalName(coll[i].getName());
      description[i] = marginalName;
    }
  }
  setParallel(parallel);
  // Everything is ok, store the collection
  distributionCollection_ = coll;
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  setDescription(description);
  setRange(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));
}


/* Distribution collection accessor */
const ComposedDistribution::DistributionCollection & ComposedDistribution::getDistributionCollection() const
{
  return distributionCollection_;
}


/* Copula accessor */
void ComposedDistribution::setCopula(const Distribution & copula)
{
  // We check if the copula has a dimension compatible with the one of the object,
  // especially if the object has already been created and has a collection of distribution
  if (getDimension() != 0)
  {
    if (getDimension() != copula.getDimension())
      throw InvalidArgumentException(HERE) << "Copula has a dimension different from the ComposedDistribution's";
  }
  else
    setDimension(copula.getDimension());

  copula_ = copula;
  isAlreadyComputedCovariance_ = false;
  // We ensure that the copula has the same description than the ComposedDistribution
  copula_.setDescription(getDescription());
}


/* Copula accessor */
ComposedDistribution::Implementation ComposedDistribution::getCopula() const
{
  return copula_.getImplementation();
}

/* Virtual constructor */
ComposedDistribution * ComposedDistribution::clone() const
{
  return new ComposedDistribution(*this);
}

/* Get one realization of the ComposedDistribution */
NumericalPoint ComposedDistribution::getRealization() const
{
  const UnsignedInteger dimension(getDimension());
  if (dimension == 1) return distributionCollection_[0].getRealization();
  // Special case for independent copula
  NumericalPoint result(dimension);
  if (hasIndependentCopula())
  {
    for (UnsignedInteger i = 0; i < dimension; ++i) result[i] = distributionCollection_[i].getRealization()[0];
    return result;
  }
  // General case
  const NumericalPoint realization(copula_.getRealization());
  for (UnsignedInteger i = 0; i < dimension; ++i) result[i] = distributionCollection_[i].computeQuantile(realization[i])[0];
  return result;
}

struct ComposedDistributionComputeSamplePolicy
{
  const NumericalSample input_;
  NumericalSample & output_;
  const Collection<Distribution> distributionCollection_;
  UnsignedInteger dimension_;

  ComposedDistributionComputeSamplePolicy(const NumericalSample & input,
                                          NumericalSample & output,
                                          const Collection<Distribution> & distributionCollection)
    : input_(input)
    , output_(output)
    , distributionCollection_(distributionCollection)
    , dimension_(distributionCollection.getSize())
  {}

  inline void operator()( const TBB::BlockedRange<UnsignedInteger> & r ) const
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i)
      for (UnsignedInteger j = 0; j < dimension_; ++j)
        output_[i][j] = distributionCollection_[j].computeQuantile(input_[i][j])[0];
  }

}; /* end struct ComposedDistributionComputeSamplePolicy */

/* Get a sample of the distribution */
NumericalSample ComposedDistribution::getSampleParallel(const UnsignedInteger size) const
{
  const UnsignedInteger dimension(getDimension());
  // For 1D or independent components, we can only rely on possible parallel
  // implementation of getSample() methods of the marginal distributions
  if (dimension == 1) return distributionCollection_[0].getSample(size);
  // Special case for independent copula
  if (hasIndependentCopula())
  {
    NumericalSample result(size, dimension);
    for (UnsignedInteger i = 0; i < dimension; ++i)
    {
      const NumericalSample marginalSample(distributionCollection_[i].getSample(size));
      for (UnsignedInteger j = 0; j < size; ++j) result[j][i] = marginalSample[j][0];
    }
    result.setName(getName());
    result.setDescription(getDescription());
    return result;
  }
  // For dependent components, we can use some parallelism on top
  // of possible parallelism of the getSample() method of the copula
  const NumericalSample copulaSample(copula_.getSample(size));
  NumericalSample result(size, dimension);
  const ComposedDistributionComputeSamplePolicy policy( copulaSample, result, distributionCollection_ );
  TBB::ParallelFor( 0, size, policy );
  result.setName(getName());
  result.setDescription(getDescription());
  return result;
}

NumericalSample ComposedDistribution::getSample(const UnsignedInteger size) const
{
  if (isParallel_) return getSampleParallel(size);
  return DistributionImplementation::getSample(size);
}

/* Get the DDF of the ComposedDistribution */
NumericalPoint ComposedDistribution::computeDDF(const NumericalPoint & point) const
{
  /* PDF = PDF_copula(CDF_dist1(p1), ..., CDF_distn(pn))xPDF_dist1(p1)x...xPDF_distn(pn) */
  const UnsignedInteger dimension(getDimension());
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  NumericalPoint uPoint(dimension);
  NumericalPoint pdfMarginal(dimension);
  NumericalPoint ddfMarginal(dimension);
  NumericalScalar productPDF(1.0);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const NumericalPoint component(NumericalPoint(1, point[i]));
    uPoint[i] = distributionCollection_[i].computeCDF(component);
    pdfMarginal[i] = distributionCollection_[i].computePDF(component);
    ddfMarginal[i] = distributionCollection_[i].computeDDF(component)[0];
    productPDF *= pdfMarginal[i];
  }
  // Initialization with the values of an independent copula
  NumericalScalar pdfCopula(1.0);
  NumericalPoint ddfCopula(dimension, 0.0);
  // If the distribution does not have an independent copula
  if (!hasIndependentCopula())
  {
    pdfCopula = copula_.computePDF(uPoint);
    ddfCopula = copula_.computeDDF(uPoint);
  }
  // Compute the ddf
  NumericalPoint ddf(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i) if (pdfMarginal[i] > 0.0) ddf[i] = productPDF * (ddfCopula[i] * pdfMarginal[i] + pdfCopula * ddfMarginal[i] / pdfMarginal[i]);
  return ddf;
}

/* Get the PDF of the ComposedDistribution */
NumericalScalar ComposedDistribution::computePDF(const NumericalPoint & point) const
{
  /* PDF = PDF_copula(CDF_dist1(p1), ..., CDF_distn(pn))xPDF_dist1(p1)x...xPDF_distn(pn) */
  const UnsignedInteger dimension(getDimension());
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  // Special case for dimension 1, to boost performances
  if (dimension == 1) return distributionCollection_[0].computePDF(point);
  NumericalScalar productPDF(1.0);
  // Special case for the independent case, to boost performances
  if (hasIndependentCopula())
  {
    for (UnsignedInteger i = 0; i < dimension; ++i) productPDF *= distributionCollection_[i].computePDF(point[i]);
    return productPDF;
  }
  // General case
  NumericalPoint uPoint(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const NumericalPoint component(NumericalPoint(1, point[i]));
    uPoint[i] = distributionCollection_[i].computeCDF(component);
    productPDF *= distributionCollection_[i].computePDF(component);
  }
  return copula_.computePDF(uPoint) * productPDF;
}

/* Get the CDF of the ComposedDistribution */
NumericalScalar ComposedDistribution::computeCDF(const NumericalPoint & point) const
{
  /* CDF = CDF_copula(CDF_dist1(p1), ..., CDF_distn(pn)) */
  const UnsignedInteger dimension(getDimension());
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  if (dimension == 1) return distributionCollection_[0].computeCDF(point);
  // Special case for the independent case, to boost performances
  if (hasIndependentCopula())
  {
    NumericalScalar productCDF(1.0);
    for (UnsignedInteger i = 0; i < dimension; ++i) productCDF *= distributionCollection_[i].computeCDF(point[i]);
    return productCDF;
  }
  // General case
  NumericalPoint uPoint(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i) uPoint[i] = distributionCollection_[i].computeCDF(point[i]);
  return copula_.computeCDF(uPoint);
}

NumericalScalar ComposedDistribution::computeSurvivalFunction(const NumericalPoint & point) const
{
  /* Survival = copula_survival(dist1_survival(p1), ..., distn_survival(pn)) */
  const UnsignedInteger dimension(getDimension());
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  if (dimension == 1) return distributionCollection_[0].computeSurvivalFunction(point);
  // Special case for the independent case, to boost performances
  if (hasIndependentCopula())
  {
    NumericalScalar productSurvival(1.0);
    for (UnsignedInteger i = 0; i < dimension; ++i) productSurvival *= distributionCollection_[i].computeSurvivalFunction(point[i]);
    return productSurvival;
  }
  // General case
  NumericalPoint uPoint(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i) uPoint[i] = distributionCollection_[i].computeSurvivalFunction(point[i]);
  return copula_.computeSurvivalFunction(uPoint);
}

/* Compute the probability content of an interval */
NumericalScalar ComposedDistribution::computeProbability(const Interval & interval) const
{
  const UnsignedInteger dimension(getDimension());
  if (interval.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given interval must have dimension=" << dimension << ", here dimension=" << interval.getDimension();

  // If the interval is empty
  if (interval.isNumericallyEmpty()) return 0.0;
  const NumericalPoint lower(interval.getLowerBound());
  const NumericalPoint upper(interval.getUpperBound());
  const Interval::BoolCollection finiteLower(interval.getFiniteLowerBound());
  const Interval::BoolCollection finiteUpper(interval.getFiniteUpperBound());
  NumericalPoint lowerCopula(dimension);
  NumericalPoint upperCopula(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    if (finiteLower[i]) lowerCopula[i] = distributionCollection_[i].computeCDF(lower[i]);
    else lowerCopula[i] = 0.0;
    if (finiteUpper[i]) upperCopula[i] = distributionCollection_[i].computeCDF(upper[i]);
    else upperCopula[i] = 1.0;
  }
  return copula_.computeProbability(Interval(lowerCopula, upperCopula));
}

/* Get the PDF gradient of the distribution */
NumericalPoint ComposedDistribution::computePDFGradient(const NumericalPoint & point) const
{
  const UnsignedInteger dimension(getDimension());
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  NumericalPoint gradient;
  // First, put the gradient according to marginal parameters
  // The marginal parameters are supposed to be independent from one marginal distribution
  // to the others
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const NumericalPoint marginalGradient(distributionCollection_[i].computePDFGradient(NumericalPoint(1, point[i])));
    const UnsignedInteger marginalParameterDimension(marginalGradient.getDimension());
    for (UnsignedInteger j = 0; j < marginalParameterDimension; ++j) gradient.add(marginalGradient[j]);
  }
  const NumericalPoint copulaGradient(copula_.computePDFGradient(point));
  const UnsignedInteger copulaParameterDimension(copulaGradient.getDimension());
  // Then, put the gradient according to the copula parameters
  for (UnsignedInteger j = 0; j < copulaParameterDimension; ++j) gradient.add(copulaGradient[j]);
  return gradient;
}

/* Get the CDF gradient of the distribution */
NumericalPoint ComposedDistribution::computeCDFGradient(const NumericalPoint & point) const
{
  const UnsignedInteger dimension(getDimension());
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  NumericalPoint gradient;
  // First, put the gradient according to marginal parameters
  // The marginal parameters are supposed to be independent from one marginal distribution
  // to the others
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const NumericalPoint marginalGradient(distributionCollection_[i].computeCDFGradient(NumericalPoint(1, point[i])));
    const UnsignedInteger marginalParameterDimension(marginalGradient.getDimension());
    for (UnsignedInteger j = 0; j < marginalParameterDimension; ++j) gradient.add(marginalGradient[j]);
  }
  const NumericalPoint copulaGradient(copula_.computeCDFGradient(point));
  const UnsignedInteger copulaParameterDimension(copulaGradient.getDimension());
  // Then, put the gradient according to the copula parameters
  for (UnsignedInteger j = 0; j < copulaParameterDimension; ++j) gradient.add(copulaGradient[j]);
  return gradient;
}

/* Get the quantile of the ComposedDistribution */
NumericalPoint ComposedDistribution::computeQuantile(const NumericalScalar prob,
    const Bool tail) const
{
  if (prob < 0.0 || prob > 1.0) throw InvalidArgumentException(HERE) << "Error: cannot compute a quantile for a probability level outside of [0, 1]";
  const UnsignedInteger dimension(getDimension());
  if (dimension == 1) return distributionCollection_[0].computeQuantile(prob, tail);
  NumericalPoint quantile(copula_.computeQuantile(prob));
  for (UnsignedInteger i = 0; i < getDimension(); ++i) quantile[i] = distributionCollection_[i].computeQuantile(quantile[i])[0];
  return quantile;
}

/* Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
NumericalScalar ComposedDistribution::computeConditionalPDF(const NumericalScalar x, const NumericalPoint & y) const
{
  const UnsignedInteger conditioningDimension(y.getDimension());
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional PDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  // Special case for no conditioning or independent copula
  if ((conditioningDimension == 0) || (hasIndependentCopula())) return Distribution(getMarginal(conditioningDimension)).computePDF(x);
  // General case
  NumericalPoint u(conditioningDimension);
  for (UnsignedInteger i = 0; i < conditioningDimension; ++i) u[i] = distributionCollection_[i].computeCDF(y[i]);
  return distributionCollection_[conditioningDimension].computePDF(x) * copula_.computeConditionalPDF(distributionCollection_[conditioningDimension].computeCDF(x), u);
}

/* Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
NumericalScalar ComposedDistribution::computeConditionalCDF(const NumericalScalar x, const NumericalPoint & y) const
{
  const UnsignedInteger conditioningDimension(y.getDimension());
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional CDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  // Special case for no conditioning or independent copula
  if ((conditioningDimension == 0) || (hasIndependentCopula())) return distributionCollection_[conditioningDimension].computeCDF(x);
  // General case
  NumericalPoint u(conditioningDimension);
  for (UnsignedInteger i = 0; i < conditioningDimension; ++i) u[i] = distributionCollection_[i].computeCDF(y[i]);
  return copula_.computeConditionalCDF(distributionCollection_[conditioningDimension].computeCDF(x), u);
}

/* Compute the numerical range of the distribution given the parameters values */
void ComposedDistribution::computeRange()
{
  const UnsignedInteger dimension(getDimension());
  NumericalPoint lowerBound(dimension);
  NumericalPoint upperBound(dimension);
  Interval::BoolCollection finiteLowerBound(dimension);
  Interval::BoolCollection finiteUpperBound(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const Interval atomRange(distributionCollection_[i].getRange());
    lowerBound[i] = atomRange.getLowerBound()[0];
    upperBound[i] = atomRange.getUpperBound()[0];
    finiteLowerBound[i] = atomRange.getFiniteLowerBound()[0];
    finiteUpperBound[i] = atomRange.getFiniteUpperBound()[0];
  }
  setRange(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));
}

/* Compute the mean of the distribution. It is cheap if the marginal means are cheap */
void ComposedDistribution::computeMean() const
{
  const UnsignedInteger dimension(getDimension());
  mean_ = NumericalPoint(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i) mean_[i] = distributionCollection_[i].getMean()[0];
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
NumericalPoint ComposedDistribution::getStandardDeviation() const
{
  const UnsignedInteger dimension(getDimension());
  NumericalPoint standardDeviation(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i) standardDeviation[i] = distributionCollection_[i].getStandardDeviation()[0];
  return standardDeviation;
}

/* Compute the covariance of the distribution */
void ComposedDistribution::computeCovariance() const
{
  const UnsignedInteger dimension(getDimension());
  // We need this to initialize the covariance matrix in two cases:
  // + this is the first call to this routine (which could be checked by testing the dimension of the distribution and the dimension of the matrix
  // + the copula has changed from a non-independent one to the independent copula
  covariance_ = CovarianceMatrix(dimension);
  // First the diagonal terms, which are the marginal covariances
  // Compute the marginal standard deviation
  const NumericalPoint sigma(getStandardDeviation());
  for(UnsignedInteger component = 0; component < dimension; ++component) covariance_(component, component) = sigma[component] * sigma[component];
  // Off-diagonal terms if the copula is not the independent copula
  if (!hasIndependentCopula())
  {
    // Special case: elliptical distribution. The covariance is the shape matrix of the associated copula, scaled by the marginal standard deviations
    if (isElliptical())
    {
      const CovarianceMatrix shape(copula_.getShapeMatrix());
      for (UnsignedInteger rowIndex = 0; rowIndex < dimension; ++rowIndex)
        for (UnsignedInteger columnIndex = rowIndex + 1; columnIndex < dimension; ++columnIndex)
          covariance_(rowIndex, columnIndex) = shape(rowIndex, columnIndex) * sigma[rowIndex] * sigma[columnIndex];
      return;
    }
    // To ensure that the mean is up to date
    mean_ = getMean();
    // Compute the weights and nodes of the 1D gauss quadrature over [-1, 1]
    NumericalPoint gaussWeights;
    NumericalPoint gaussNodes(getGaussNodesAndWeights(gaussWeights));
    // Convert the nodes and weights for the interval [0, 1]
    for (UnsignedInteger i = 0; i < integrationNodesNumber_; ++i)
    {
      gaussNodes[i] = 0.5 * (gaussNodes[i] + 1.0);
      gaussWeights[i] *= 0.5;
    }
    // Compute the marginal quantiles at the nodes
    NumericalSample marginalQuantiles(integrationNodesNumber_, dimension);
    NumericalSample marginalPDF(integrationNodesNumber_, dimension);
    for(UnsignedInteger component = 0; component < dimension; ++component)
    {
      const Distribution marginalDistribution(getMarginal(component));
      for(UnsignedInteger nodeIndex = 0; nodeIndex < integrationNodesNumber_; ++nodeIndex)
      {
        const NumericalScalar node(gaussNodes[nodeIndex]);
        const NumericalPoint q(marginalDistribution.computeQuantile(node));
        marginalQuantiles[nodeIndex][component] = q[0];
        marginalPDF[nodeIndex][component] = marginalDistribution.computePDF(q);
      }
    }
    // Performs the integration for each covariance in the strictly lower triangle of the covariance matrix
    // We simply use a product gauss quadrature
    // We first loop over the coeeficients because the most expensive task is to get the 2D marginal copulas
    Indices indices(2);
    for(UnsignedInteger rowIndex = 0; rowIndex < dimension; ++rowIndex)
    {
      indices[0] = rowIndex;
      // We must fill the upper triangle of the covariance matrix in order to access the 2D marginal distributions
      // of the copula in the correct order for the ComposedCopula
      for(UnsignedInteger columnIndex = rowIndex + 1; columnIndex < dimension; ++columnIndex)
      {
        indices[1] = columnIndex;
        const Distribution marginalCopula(copula_.getMarginal(indices));
        if (!marginalCopula.hasIndependentCopula())
        {
          NumericalScalar covarianceIJ(0.0);
          // Then we loop over the integration points
          for(UnsignedInteger rowNodeIndex = 0; rowNodeIndex < integrationNodesNumber_; ++rowNodeIndex)
          {
            const NumericalScalar nodeI(gaussNodes[rowNodeIndex]);
            const NumericalScalar weightI(gaussWeights[rowNodeIndex]);
            for(UnsignedInteger columnNodeIndex = 0; columnNodeIndex < integrationNodesNumber_; ++columnNodeIndex)
            {
              const NumericalScalar nodeJ(gaussNodes[columnNodeIndex]);
              const NumericalScalar weightJ(gaussWeights[columnNodeIndex]);
              NumericalPoint in(2);
              in[0] = nodeI;
              in[1] = nodeJ;
              covarianceIJ += weightI * weightJ * (marginalQuantiles[rowNodeIndex][rowIndex] - mean_[rowIndex]) *
                              (marginalQuantiles[columnNodeIndex][columnIndex] - mean_[columnIndex]) * marginalCopula.computePDF(in);
            } // loop over J integration nodes
          } // loop over I integration nodes
          covariance_(rowIndex, columnIndex) = covarianceIJ;
        }
      } // loop over column indices
    } // loop over row indices
  } // if !hasIndependentCopula
  isAlreadyComputedCovariance_ = true;
} // computeCovariance

/* Get the skewness of the distribution */
NumericalPoint ComposedDistribution::getSkewness() const
{
  const UnsignedInteger dimension(getDimension());
  NumericalPoint skewness(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i) skewness[i] = distributionCollection_[i].getSkewness()[0];
  return skewness;
}

/* Get the kurtosis of the distribution */
NumericalPoint ComposedDistribution::getKurtosis() const
{
  const UnsignedInteger dimension(getDimension());
  NumericalPoint kurtosis(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i) kurtosis[i] = distributionCollection_[i].getKurtosis()[0];
  return kurtosis;
}

/* Get the i-th marginal distribution */
ComposedDistribution::Implementation ComposedDistribution::getMarginal(const UnsignedInteger i) const
{
  if (i >= getDimension()) throw InvalidArgumentException(HERE) << "The index of a marginal distribution must be in the range [0, dim-1]";
  Distribution marginal(distributionCollection_[i]);
  marginal.setDescription(Description(1, getDescription()[i]));
  return marginal.getImplementation()->clone();
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
ComposedDistribution::Implementation ComposedDistribution::getMarginal(const Indices & indices) const
{
  // This call will check that indices are correct
  const Copula marginalCopula(copula_.getMarginal(indices));
  DistributionCollection marginalDistributions(0);
  const UnsignedInteger size(indices.getSize());
  const Description description(getDescription());
  Description marginalDescription(size);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const UnsignedInteger j(indices[i]);
    marginalDistributions.add(distributionCollection_[j]);
    marginalDescription[i] = description[j];
  }
  ComposedDistribution marginal(marginalDistributions, marginalCopula);
  marginal.setDescription(marginalDescription);
  return marginal.clone();
}

/* Get the isoprobabilistic transformation */
ComposedDistribution::IsoProbabilisticTransformation ComposedDistribution::getIsoProbabilisticTransformation() const
{
  const UnsignedInteger dimension(getDimension());
  // Set the parameters values and descriptions
  NumericalPointWithDescriptionCollection parametersCollection(getParametersCollection());
  // First, compute the dimension of the marginal parameters space
  const UnsignedInteger size(parametersCollection.getSize());
  NumericalPointWithDescription parameters(0);
  Description description(0);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const NumericalPointWithDescription marginalParameters(parametersCollection[i]);
    const Description marginalDescription(marginalParameters.getDescription());
    const UnsignedInteger marginalDimension(marginalParameters.getDimension());
    const String marginalName(marginalParameters.getName());
    for (UnsignedInteger j = 0; j < marginalDimension; ++j)
    {
      parameters.add(marginalParameters[j]);
      description.add(OSS() << marginalName << "_" << marginalDescription[j]);
    }
  }
  parameters.setDescription(description);
  // Special case for the independent copula case: marginal transformations only to go to the spherical distribution
  if (hasIndependentCopula())
  {
    // Get the evaluation implementations
    MarginalTransformationEvaluation evaluation(distributionCollection_, DistributionCollection(dimension, Normal()));
    // We have to correct the direction because the output collection corresponds to the standard space, so there is no parameter to take into account for these distributions
    evaluation.setDirection(MarginalTransformationEvaluation::FROM);
    IsoProbabilisticTransformation marginalTransformation(evaluation.clone(), new MarginalTransformationGradient(evaluation), new MarginalTransformationHessian(evaluation));
    marginalTransformation.setParameters(parameters);
    return marginalTransformation;
  }
  // Special case for the elliptical distribution case: linear transformation
  if (isElliptical())
  {
    const TriangularMatrix inverseCholesky(getInverseCholesky());
    const NumericalPoint mean(getMean());
    IsoProbabilisticTransformation transform;
    transform.setEvaluation(new NatafEllipticalDistributionEvaluation(mean, inverseCholesky));
    transform.setGradient(new NatafEllipticalDistributionGradient(inverseCholesky));
    transform.setHessian(new NatafEllipticalDistributionHessian(dimension));
    transform.setParameters(parameters);
    return transform;
  }
  // Special case for the elliptical copula case: generalized Nataf transformation (marginal transformations plus linear transformation)
  if (hasEllipticalCopula())
  {
    // Standard distribution
    const Distribution standardDistribution(getStandardDistribution());
    // Get the evaluation implementations
    const Distribution standardMarginal(standardDistribution.getMarginal(0));
    MarginalTransformationEvaluation evaluation(distributionCollection_, DistributionCollection(dimension, standardMarginal));
    // We have to correct the direction because the output collection corresponds to the standard space, so there is no parameter to take into account for these distributions
    evaluation.setDirection(MarginalTransformationEvaluation::FROM);
    IsoProbabilisticTransformation marginalTransformation(evaluation.clone(), MarginalTransformationGradient(evaluation).clone(), MarginalTransformationHessian(evaluation).clone());
    marginalTransformation.setParameters(parameters);
    // Suppress the correlation between the components.
    const TriangularMatrix inverseCholesky(copula_.getShapeMatrix().computeCholesky().solveLinearSystem(IdentityMatrix(dimension)).getImplementation());
    LinearNumericalMathFunction linear(NumericalPoint(dimension, 0.0), NumericalPoint(dimension, 0.0), inverseCholesky);
    return IsoProbabilisticTransformation(linear, marginalTransformation);
  }
  // General case: go to uniform marginal distributions using marginal transformations, then use the isoprobabilistic ransformation of the copula
  // Get the IsoProbabilisticTransformation from the copula
  const IsoProbabilisticTransformation copulaIsoprobabilisticTransformation(copula_.getIsoProbabilisticTransformation());
  // Get the right function implementations
  const MarginalTransformationEvaluation evaluation(distributionCollection_, MarginalTransformationEvaluation::FROM);
  IsoProbabilisticTransformation marginalTransformation(evaluation.clone(), new MarginalTransformationGradient(evaluation), new MarginalTransformationHessian(evaluation));
  marginalTransformation.setParameters(parameters);
  return IsoProbabilisticTransformation(copulaIsoprobabilisticTransformation, marginalTransformation);
}

/* Get the inverse isoprobabilist transformation */
ComposedDistribution::InverseIsoProbabilisticTransformation ComposedDistribution::getInverseIsoProbabilisticTransformation() const
{
  const UnsignedInteger dimension(getDimension());
  // Set the parameters values and descriptions
  NumericalPointWithDescriptionCollection parametersCollection(getParametersCollection());
  // First, compute the dimension of the marginal parameters space
  const UnsignedInteger size(parametersCollection.getSize());
  NumericalPointWithDescription parameters(0);
  Description description(0);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const NumericalPointWithDescription marginalParameters(parametersCollection[i]);
    const Description marginalDescription(marginalParameters.getDescription());
    const UnsignedInteger marginalDimension(marginalParameters.getDimension());
    const String marginalName(marginalParameters.getName());
    for (UnsignedInteger j = 0; j < marginalDimension; ++j)
    {
      parameters.add(marginalParameters[j]);
      description.add(OSS() << marginalName << "_" << marginalDescription[j]);
    }
  }
  parameters.setDescription(description);
  // Special case for the independent copula case: marginal transformations only to go back from the spherical distribution
  if (hasIndependentCopula())
  {
    // Get the evaluation implementations
    MarginalTransformationEvaluation evaluation(DistributionCollection(dimension, Normal()), distributionCollection_);
    // We have to correct the direction because the input collection corresponds to the standard space, so there is no parameter to take into account for these distributions
    evaluation.setDirection(MarginalTransformationEvaluation::TO);
    IsoProbabilisticTransformation marginalTransformation(evaluation.clone(), new MarginalTransformationGradient(evaluation), new MarginalTransformationHessian(evaluation));
    marginalTransformation.setParameters(parameters);
    return marginalTransformation;
  }
  // Special case for the elliptical distribution case: linear transformation
  if (isElliptical())
  {
    const TriangularMatrix cholesky(getCholesky());
    const NumericalPoint mean(getMean());
    InverseIsoProbabilisticTransformation inverseTransform;
    inverseTransform.setEvaluation(new InverseNatafEllipticalDistributionEvaluation(mean, cholesky));
    inverseTransform.setGradient(new InverseNatafEllipticalDistributionGradient(cholesky));
    inverseTransform.setHessian(new InverseNatafEllipticalDistributionHessian(dimension));
    inverseTransform.setParameters(parameters);
    return inverseTransform;
  }
  // Special case for the elliptical copula case: generalized Nataf transformation (marginal transformations plus linear transformation)
  if (hasEllipticalCopula())
  {
    // Standard distribution
    const Distribution standardDistribution(getStandardDistribution());
    // Get the evaluation implementations
    const Distribution standardMarginal(standardDistribution.getMarginal(0));
    MarginalTransformationEvaluation evaluation(DistributionCollection(dimension, standardMarginal), distributionCollection_);
    // We have to correct the direction because the output collection corresponds to the standard space, so there is no parameter to take into account for these distributions
    evaluation.setDirection(MarginalTransformationEvaluation::TO);
    InverseIsoProbabilisticTransformation marginalTransformation(evaluation.clone(), new MarginalTransformationGradient(evaluation), new MarginalTransformationHessian(evaluation));
    marginalTransformation.setParameters(parameters);
    // Suppress the correlation between the components.
    const TriangularMatrix cholesky(copula_.getShapeMatrix().computeCholesky());
    // const SquareMatrix cholesky(ComposedDistribution(DistributionCollection(dimension, standardMarginal), getCopula()).getCholesky());
    LinearNumericalMathFunction linear(NumericalPoint(dimension, 0.0), NumericalPoint(dimension, 0.0), cholesky);
    return InverseIsoProbabilisticTransformation(marginalTransformation, linear);
  }
  // General case: go to the copula using its inverse isoprobabilistic transformation, then add the correct marginal distributions using marginal transformations
  // Get the InverseIsoProbabilisticTransformation from the copula
  const InverseIsoProbabilisticTransformation copulaInverseIsoprobabilisticTransformation(copula_.getInverseIsoProbabilisticTransformation());
  // Build the marginal transformation
  const MarginalTransformationEvaluation evaluation(distributionCollection_, MarginalTransformationEvaluation::TO);
  InverseIsoProbabilisticTransformation marginalTransformation(evaluation.clone(), new MarginalTransformationGradient(evaluation), new MarginalTransformationHessian(evaluation));
  marginalTransformation.setParameters(parameters);
  return InverseIsoProbabilisticTransformation(marginalTransformation, copulaInverseIsoprobabilisticTransformation);
}

/* Get the standard distribution */
ComposedDistribution::Implementation ComposedDistribution::getStandardDistribution() const
{
  return copula_.getStandardDistribution().getImplementation();
}

/* Parameters value and description accessor */
ComposedDistribution::NumericalPointWithDescriptionCollection ComposedDistribution::getParametersCollection() const
{
  const UnsignedInteger dimension(getDimension());
  NumericalPointWithDescriptionCollection parameters(dimension + (dimension > 1 ? 1 : 0));
  const Description description(getDescription());
  // First put the marginal parameters
  for (UnsignedInteger marginalIndex = 0; marginalIndex < dimension; ++marginalIndex)
  {
    // Each marginal distribution must output a collection of parameters of size 1, even if it contains an empty NumericalPoint
    const NumericalPointWithDescriptionCollection marginalParameters(distributionCollection_[marginalIndex].getParametersCollection());
    NumericalPointWithDescription point(marginalParameters[0]);
    Description marginalParametersDescription(point.getDescription());
    // Here we must add a unique prefix to the marginal parameters description in order to deambiguate the parameters of different marginals sharing the same description
    for (UnsignedInteger i = 0; i < point.getDimension(); ++i) marginalParametersDescription[i] = (OSS() << marginalParametersDescription[i] << "_marginal_" << marginalIndex);
    point.setDescription(marginalParametersDescription);
    point.setName(description[marginalIndex]);
    parameters[marginalIndex] = point;
  } // marginalIndex
  if (dimension > 1)
  {
    // Second put the dependence parameters
    NumericalPointWithDescription point(copula_.getParametersCollection()[0]);
    Description copulaParametersDescription(point.getDescription());
    // Here we must add a unique prefix to the copula parameters description in order to deambiguate the parameters of marginals sharing the same description
    for (UnsignedInteger i = 0; i < point.getDimension(); ++i) copulaParametersDescription[i] = (OSS() << copulaParametersDescription[i] << "_copula");
    point.setDescription(copulaParametersDescription);
    point.setName(copula_.getName());
    parameters[dimension] = point;
  } // dimension > 1
  return parameters;
} // getParametersCollection


void ComposedDistribution::setParametersCollection(const NumericalPointCollection& parametersCollection)
{
  const UnsignedInteger dimension(getDimension());
  const UnsignedInteger parametersSize(dimension + (dimension > 1 ? 1 : 0));
  if (parametersCollection.getSize() < parametersSize) throw InvalidArgumentException(HERE) << "The collection is too small(" << parametersCollection.getSize() << "). Expected (" << parametersSize << ")";

  // set marginal parameters
  for (UnsignedInteger marginalIndex = 0; marginalIndex < dimension; ++marginalIndex) distributionCollection_[marginalIndex].setParameters(parametersCollection[marginalIndex]);
  // set copula parameters
  if (dimension > 1) copula_.setParameters(parametersCollection[dimension]);
}


NumericalPoint ComposedDistribution::getParameters() const
{
  const UnsignedInteger dimension = getDimension();
  NumericalPoint point;
  for (UnsignedInteger marginalIndex = 0; marginalIndex < dimension; ++ marginalIndex)
  {
    point.add(distributionCollection_[marginalIndex].getParameters());
  }
  if (dimension > 1)
  {
    point.add(copula_.getParameters());
  }
  return point;
}

void ComposedDistribution::setParameters(const NumericalPoint & parameters)
{
  const UnsignedInteger dimension = getDimension();
  UnsignedInteger globalIndex = 0;
  for (UnsignedInteger marginalIndex = 0; marginalIndex < dimension; ++ marginalIndex)
  {
    const UnsignedInteger parametersSize = distributionCollection_[marginalIndex].getParameters().getSize();
    if (globalIndex + parametersSize > parameters.getSize()) throw InvalidArgumentException(HERE) << "Not enough values (" << parameters.getSize() << "), needed " << globalIndex + parametersSize << " for marginal " << marginalIndex;
    NumericalPoint newParameters(parametersSize);
    std::copy(parameters.begin() + globalIndex, parameters.begin() + globalIndex + parametersSize, newParameters.begin());
    distributionCollection_[marginalIndex].setParameters(newParameters);
    globalIndex += parametersSize;
  }
  if (dimension > 1)
  {
    const UnsignedInteger parametersSize = copula_.getParameters().getSize();
    if (globalIndex + parametersSize > parameters.getSize()) throw InvalidArgumentException(HERE) << "Not enough values (" << parameters.getSize() << "), needed " << globalIndex + parametersSize << " for copula";
    NumericalPoint newParameters(parametersSize);
    std::copy(parameters.begin() + globalIndex, parameters.begin() + globalIndex + parametersSize, newParameters.begin());
    copula_.setParameters(newParameters);
  }
}

Description ComposedDistribution::getParametersDescription() const
{
  const UnsignedInteger dimension = getDimension();
  Description description;
  for (UnsignedInteger marginalIndex = 0; marginalIndex < dimension; ++ marginalIndex)
  {
    Description marginalParametersDescription(distributionCollection_[marginalIndex].getParametersDescription());
    for (UnsignedInteger i = 0; i < marginalParametersDescription.getSize(); ++ i)
      description.add(OSS() << marginalParametersDescription[i] << "_marginal_" << marginalIndex);
  }
  if (dimension > 1)
  {
    Description copulaParametersDescription(copula_.getParametersDescription());
    for (UnsignedInteger i = 0; i < copulaParametersDescription.getSize(); ++ i)
      description.add(OSS() << copulaParametersDescription[i] << "_copula");
  }
  return description;
}

/* Tell if the distribution has independent copula */
Bool ComposedDistribution::hasIndependentCopula() const
{
  return copula_.hasIndependentCopula();
}

/* Tell if the distribution has elliptical copula */
Bool ComposedDistribution::hasEllipticalCopula() const
{
  return copula_.hasEllipticalCopula();
}

/* Check if the distribution is elliptical */
Bool ComposedDistribution::isElliptical() const
{
  const Bool ellipticalCopula(copula_.hasEllipticalCopula());
  if (!ellipticalCopula) return false;
  const String copulaKind(copula_.getImplementation()->getClassName());
  // Easy case: Normal or independent copula with Normal marginals
  const Bool hasNormalCopula((copulaKind == NormalCopula::GetClassName()) || hasIndependentCopula());
  Bool hasNormalMarginals(true);
  for (UnsignedInteger i = 0; i < getDimension(); ++i)
  {
    const String currentMarginalKind(distributionCollection_[i].getImplementation()->getClassName());
    hasNormalMarginals = hasNormalMarginals && (currentMarginalKind == Normal::GetClassName());
  }
  // For now, we are not smart enough to detect a fully general elliptical copula. The general
  // way to do it is to compare the density generator of the marginals and the copula.
  return hasNormalCopula && hasNormalMarginals;
}

/* Check if the distribution is continuous */
Bool ComposedDistribution::isContinuous() const
{
  const UnsignedInteger dimension(getDimension());
  for (UnsignedInteger i = 0; i < dimension; ++i) if (!distributionCollection_[i].isContinuous()) return false;
  return true;
}

/* Check if the distribution is discrete */
Bool ComposedDistribution::isDiscrete() const
{
  const UnsignedInteger dimension(getDimension());
  for (UnsignedInteger i = 0; i < dimension; ++i) if (!distributionCollection_[i].isDiscrete()) return false;
  return true;
}

/* Tell if the distribution is integer valued */
Bool ComposedDistribution::isIntegral() const
{
  const UnsignedInteger dimension(getDimension());
  for (UnsignedInteger i = 0; i < dimension; ++i) if (!distributionCollection_[i].isIntegral()) return false;
  return true;
}

/* Method save() stores the object through the StorageManager */
void ComposedDistribution::save(Advocate & adv) const
{
  DistributionImplementation::save(adv);
  adv.saveAttribute( "distributionCollection_", distributionCollection_ );
  adv.saveAttribute( "copula_", copula_ );
}

/* Method load() reloads the object from the StorageManager */
void ComposedDistribution::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  adv.loadAttribute( "distributionCollection_", distributionCollection_ );
  adv.loadAttribute( "copula_", copula_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS

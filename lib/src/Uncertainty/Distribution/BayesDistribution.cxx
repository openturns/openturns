//                                               -*- C++ -*-
/**
 *  @brief The BayesDistribution distribution
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/BayesDistribution.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Uniform.hxx"
#include "openturns/ConditionalDistribution.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/IdentityFunction.hxx"
#include "openturns/SymbolicFunction.hxx"
#include "openturns/IntegrationAlgorithm.hxx"
#include "openturns/GaussKronrod.hxx"
#include "openturns/GaussLegendre.hxx"
#include "openturns/IteratedQuadrature.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(BayesDistribution)

static const Factory<BayesDistribution> Factory_BayesDistribution;


/* Default constructor */
BayesDistribution::BayesDistribution()
  : ContinuousDistribution()
  , conditionedDistribution_()
  , conditioningDistribution_()
  , linkFunction_()
{
  const Description inVars(Description::BuildDefault(1, "y"));
  Description formulas(2);
  formulas[0] = inVars[0];
  formulas[1] = String(OSS() << inVars[0] << " + 1");
  setConditionedAndConditioningDistributionsAndLinkFunction(Uniform(), Uniform(), SymbolicFunction(inVars, formulas));
  setName("BayesDistribution");
  isParallel_ = false;
}

/* Parameters constructor */
BayesDistribution::BayesDistribution(const Distribution & conditionedDistribution,
                                     const Distribution & conditioningDistribution,
                                     const Function & linkFunction)
  : ContinuousDistribution()
  , conditionedDistribution_(conditionedDistribution)
  , conditioningDistribution_(conditioningDistribution)
  , linkFunction_(linkFunction)
{
  if (!conditionedDistribution.isContinuous()) throw InvalidArgumentException(HERE) << "Error: the BayesDistribution is defined only for continuous conditioned distributions, here conditionedDistribution=" << conditionedDistribution;
  if (!conditioningDistribution.isContinuous()) throw InvalidArgumentException(HERE) << "Error: the BayesDistribution is defined only for continuous conditioned distributions, here conditioningDistribution=" << conditioningDistribution;
  setConditionedAndConditioningDistributionsAndLinkFunction(conditionedDistribution, conditioningDistribution, linkFunction);
  setName("BayesDistribution");
  isParallel_ = false;
}

BayesDistribution::BayesDistribution(const Distribution & conditionedDistribution,
                                     const Distribution & conditioningDistribution)
  : ContinuousDistribution()
  , conditionedDistribution_(conditionedDistribution)
  , conditioningDistribution_(conditioningDistribution)
  , linkFunction_(IdentityFunction(conditioningDistribution.getDimension()))
{
  if (!conditionedDistribution.isContinuous()) throw InvalidArgumentException(HERE) << "Error: the BayesDistribution is defined only for continuous conditioned distributions, here conditionedDistribution=" << conditionedDistribution;
  if (!conditioningDistribution.isContinuous()) throw InvalidArgumentException(HERE) << "Error: the BayesDistribution is defined only for continuous conditioned distributions, here conditioningDistribution=" << conditioningDistribution;
  setConditionedAndConditioningDistributionsAndLinkFunction(conditionedDistribution, conditioningDistribution, linkFunction_);
  setName("BayesDistribution");
  isParallel_ = false;
}

/* Comparison operator */
Bool BayesDistribution::operator ==(const BayesDistribution & other) const
{
  if (this == &other) return true;
  return (conditionedDistribution_ == other.conditionedDistribution_) && (conditioningDistribution_ == other.conditioningDistribution_) && (linkFunction_ == other.linkFunction_);
}

Bool BayesDistribution::equals(const DistributionImplementation & other) const
{
  const BayesDistribution* p_other = dynamic_cast<const BayesDistribution*>(&other);
  return p_other && (*this == *p_other);
}

/* Compute the numerical range of the distribution given the parameters values */
void BayesDistribution::computeRange()
{
  // First, the conditioning distribution
  const Point lowerBoundConditioning(conditioningDistribution_.getRange().getLowerBound());
  const Interval::BoolCollection finiteLowerBoundConditioning(conditioningDistribution_.getRange().getFiniteLowerBound());
  const Point upperBoundConditioning(conditioningDistribution_.getRange().getUpperBound());
  const Interval::BoolCollection finiteUpperBoundConditioning(conditioningDistribution_.getRange().getFiniteUpperBound());
  // Then, the conditioned distribution
  const ConditionalDistribution deconditioned(conditionedDistribution_, conditioningDistribution_, linkFunction_);
  Point lowerBound(deconditioned.getRange().getLowerBound());
  Interval::BoolCollection finiteLowerBound(deconditioned.getRange().getFiniteLowerBound());
  Point upperBound(deconditioned.getRange().getUpperBound());
  Interval::BoolCollection finiteUpperBound(deconditioned.getRange().getFiniteUpperBound());
  // Merge everything
  lowerBound.add(lowerBoundConditioning);
  finiteLowerBound.add(finiteLowerBoundConditioning);
  upperBound.add(upperBoundConditioning);
  finiteUpperBound.add(finiteUpperBoundConditioning);

  setRange(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));
}

/* String converter */
String BayesDistribution::__repr__() const
{
  OSS oss(true);
  oss << "class=" << BayesDistribution::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " conditioned distribution=" << conditionedDistribution_
      << " conditioning distribution=" << conditioningDistribution_
      << " link function=" << linkFunction_;
  return oss;
}

String BayesDistribution::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(X, Y with X|Theta~" << conditionedDistribution_.getImplementation()->getClassName() << "(Theta), Theta=f(Y), f=" << linkFunction_.getEvaluation().__str__() << ", Y~" << conditioningDistribution_.__str__() << ")";
  return oss;
}

/* Virtual constructor */
BayesDistribution * BayesDistribution::clone() const
{
  return new BayesDistribution(*this);
}

/* Get one realization of the distribution */
Point BayesDistribution::getRealization() const
{
  const Point y(conditioningDistribution_.getRealization());
  Distribution deconditioned(conditionedDistribution_);
  deconditioned.setParameter(linkFunction_(y));
  Point x(deconditioned.getRealization());
  x.add(y);
  return x;
}

/* Get the PDF of the distribution */
Scalar BayesDistribution::computePDF(const Point & point) const
{
  if (point.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << getDimension() << ", here dimension=" << point.getDimension();

  const UnsignedInteger conditionedDimension = conditionedDistribution_.getDimension();
  const UnsignedInteger conditioningDimension = conditioningDistribution_.getDimension();
  Point y(conditioningDimension);
  std::copy(point.begin() + conditionedDimension, point.end(), y.begin());
  const Scalar conditioningPDF = conditioningDistribution_.computePDF(y);
  if (conditioningPDF == 0.0) return 0.0;
  Distribution deconditioned(conditionedDistribution_);
  const Point parameters(linkFunction_(y));
  deconditioned.setParameter(parameters);
  Point x(conditionedDimension);
  std::copy(point.begin(), point.begin() + conditionedDimension, x.begin());
  const Scalar deconditionedPDF = deconditioned.computePDF(x);
  return deconditionedPDF * conditioningPDF;
}

/* Bayes distribution accessor */
void BayesDistribution::setConditionedDistribution(const Distribution & conditionedDistribution)
{
  if (!conditionedDistribution.isContinuous()) throw InvalidArgumentException(HERE) << "Error: the BayesDistribution is defined only for continuous conditioned distributions, here conditionedDistribution=" << conditionedDistribution;
  if (conditionedDistribution != conditionedDistribution_) setConditionedAndConditioningDistributionsAndLinkFunction(conditionedDistribution, conditioningDistribution_, linkFunction_);
}

Distribution BayesDistribution::getConditionedDistribution() const
{
  return conditionedDistribution_;
}


/* Conditioning distribution accessor */
void BayesDistribution::setConditioningDistribution(const Distribution & conditioningDistribution)
{
  if (!conditioningDistribution.isContinuous()) throw InvalidArgumentException(HERE) << "Error: the BayesDistribution is defined only for continuous conditioned distributions, here conditioningDistribution=" << conditioningDistribution;
  if (conditioningDistribution != conditioningDistribution_) setConditionedAndConditioningDistributionsAndLinkFunction(conditionedDistribution_, conditioningDistribution, linkFunction_);
}

Distribution BayesDistribution::getConditioningDistribution() const
{
  return conditioningDistribution_;
}

/* Link function accessor */
void BayesDistribution::setLinkFunction(const Function & linkFunction)
{
  if (!(linkFunction == linkFunction_)) setConditionedAndConditioningDistributionsAndLinkFunction(conditionedDistribution_, conditioningDistribution_, linkFunction);
}

Function BayesDistribution::getLinkFunction() const
{
  return linkFunction_;
}

/* Method to set simultaneously the conditioning distribution, the conditioned distribution and the link function */
void BayesDistribution::setConditionedAndConditioningDistributionsAndLinkFunction(const Distribution & conditionedDistribution,
    const Distribution & conditioningDistribution,
    const Function & linkFunction)
{
  const UnsignedInteger conditioningDimension = conditioningDistribution.getDimension();
  const UnsignedInteger conditionedParametersDimension = conditionedDistribution.getParameterDimension();
  // We must check that the conditioning distribution has the same dimension as the input dimension of the link function and that the conditioning distribution has the same dimension as the input dimension of the link function
  if (conditionedParametersDimension != linkFunction.getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: expected a link function with output dimension equal to the number of parameters of the conditioned distribution.";
  if (conditioningDimension != linkFunction.getInputDimension()) throw InvalidArgumentException(HERE) << "Error: expected a link function with input dimension equal to the conditioning distribution dimension.";
  conditionedDistribution_ = conditionedDistribution;
  conditioningDistribution_ = conditioningDistribution;
  linkFunction_ = linkFunction;
  setDimension(conditioningDimension + conditionedDistribution.getDimension());
  computeRange();
}

/* Get the i-th marginal distribution */
Distribution BayesDistribution::getMarginal(const UnsignedInteger i) const
{
  if (i >= getDimension()) throw InvalidArgumentException(HERE) << "The index of a marginal distribution must be in the range [0, dim-1]";
  // Special case for dimension 1
  if (getDimension() == 1) return clone();
  // General case
  // If the index is in the conditioned part
  const UnsignedInteger conditionedDimension = conditionedDistribution_.getDimension();
  if (i < conditionedDimension) return ConditionalDistribution(conditionedDistribution_, conditioningDistribution_, linkFunction_).getMarginal(i);
  return conditioningDistribution_.getMarginal(i - conditionedDimension);
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
Distribution BayesDistribution::getMarginal(const Indices & indices) const
{
  const UnsignedInteger dimension = getDimension();
  if (!indices.check(dimension)) throw InvalidArgumentException(HERE) << "The indices of a marginal distribution must be in the range [0, dim-1] and must be different";
  // Special case for dimension 1
  if (dimension == 1) return clone();
  // General case
  // If the indices are in the conditioned part
  const UnsignedInteger conditionedDimension = conditionedDistribution_.getDimension();
  if (indices.check(conditionedDimension)) return ConditionalDistribution(conditionedDistribution_, conditioningDistribution_, linkFunction_).getMarginal(indices);
  // If the indices are in the conditioning part
  Indices conditioningIndices(0);
  const UnsignedInteger size = indices.getSize();
  for (UnsignedInteger i = 0; i < size; ++i)
    if (indices[i] >= conditionedDimension)
      conditioningIndices.add(indices[i] - conditionedDimension);
  if (conditioningIndices.getSize() == size) return conditioningDistribution_.getMarginal(conditioningIndices);
  return DistributionImplementation::getMarginal(indices);
} // getMarginal(Indices)

/* Parameters value and description accessor */
Point BayesDistribution::getParameter() const
{
  Point parameter(linkFunction_.getParameter());
  parameter.add(conditioningDistribution_.getParameter());
  return parameter;
} // getParameter

void BayesDistribution::setParameter(const Point & parameter)
{
  if (parameter.getSize() != getParameter().getSize()) throw InvalidArgumentException(HERE) << "Error: expected " << getParameter().getSize() << " values, got " << parameter.getSize();
  const Scalar w = getWeight();
  Point::const_iterator start = parameter.begin();
  const UnsignedInteger linkParameterSize = linkFunction_.getParameterDimension();
  if (linkParameterSize > 0)
  {
    Point linkParameter(linkParameterSize);
    std::copy(start, start + linkParameterSize, linkParameter.begin());
    linkFunction_.setParameter(linkParameter);
    start += linkParameterSize;
  }
  const UnsignedInteger conditioningParameterSize = parameter.getSize() - linkParameterSize;
  if (conditioningParameterSize > 0)
  {
    Point conditioningParameter(conditioningParameterSize);
    std::copy(start, parameter.end(), conditioningParameter.begin());
    conditioningDistribution_.setParameter(conditioningParameter);
  }
  *this = BayesDistribution(conditionedDistribution_, conditioningDistribution_, linkFunction_);
  setWeight(w);
} // setParameter

/* Parameters value and description accessor */
Description BayesDistribution::getParameterDescription() const
{
  Description parameterDescription(linkFunction_.getParameterDescription());
  parameterDescription.add(conditioningDistribution_.getParameterDescription());
  return parameterDescription;
} // getParameterDescription

/* Compute the mean of the distribution */
void BayesDistribution::computeMean() const
{
  const UnsignedInteger conditionedDimension = conditionedDistribution_.getDimension();
  Indices lower(conditionedDimension);
  lower.fill();
  mean_ = getMarginal(lower).getMean();
  const UnsignedInteger conditioningDimension = conditioningDistribution_.getDimension();
  Indices upper(conditioningDimension);
  upper.fill(conditionedDimension, 1);
  mean_.add(getMarginal(upper).getMean());
  isAlreadyComputedMean_ = true;
}

namespace BayesDistributionFunctions
{

class KernelCovariance: public EvaluationImplementation
{
public:
  KernelCovariance(const BayesDistribution & distribution)
    : EvaluationImplementation()
    , distribution_(distribution)
    , dimension_(distribution.getDimension())
    , conditionedDimension_(distribution.getConditionedDistribution().getDimension())
    , outputDimension_(conditionedDimension_ * (2 * dimension_ - conditionedDimension_ + 3) / 2)
    , mu_(distribution.getMean())
  {
    // Nothing to do
  }

  KernelCovariance * clone() const
  {
    return new KernelCovariance(*this);
  }

  Point operator() (const Point & point) const
  {
    Point value(outputDimension_);
    const Scalar pdf(distribution_.computePDF(point));
    UnsignedInteger index = 0;
    for (UnsignedInteger i = 0; i < conditionedDimension_; ++i)
    {
      const Scalar deltaI(point[i] - mu_[i]);
      for (UnsignedInteger j = i; j < dimension_; ++j)
      {
        const Scalar deltaJ(point[j] - mu_[j]);
        value[index] = pdf * deltaI * deltaJ;
        ++index;
      } // j
    } // i
    return value;
  }

  UnsignedInteger getInputDimension() const
  {
    return dimension_;
  }

  UnsignedInteger getOutputDimension() const
  {
    return outputDimension_;
  }

  String __repr__() const
  {
    OSS oss(true);
    oss << "class=KernelCovariance"
        << " distribution=" << distribution_
        << " dimension=" << dimension_
        << " conditionedDimension=" << conditionedDimension_
        << " outputDimension=" << outputDimension_;
    return oss;
  }

  String __str__(const String & ) const
  {
    OSS oss(false);
    oss << "KernelCovariance("
        << "distribution=" << distribution_
        << ")";
    return oss;
  }

private:
  const BayesDistribution & distribution_;
  const UnsignedInteger dimension_;
  const UnsignedInteger conditionedDimension_;
  const UnsignedInteger outputDimension_;
  const Point mu_;
}; // KernelCovariance

} // namespace BayesDistributionFunctions

/* Compute the covariance of the distribution */
void BayesDistribution::computeCovariance() const
{
  const UnsignedInteger dimension = getDimension();
  covariance_ = CovarianceMatrix(dimension);
  const UnsignedInteger conditionedDimension = conditionedDistribution_.getDimension();
  const Function integrand(BayesDistributionFunctions::KernelCovariance(*this));
  const Bool useAdaptiveAlgorithm = ResourceMap::GetAsBool("Distribution-UseCovarianceAdaptiveAlgorithm");
  IntegrationAlgorithm integrator;
  if (useAdaptiveAlgorithm) integrator = IteratedQuadrature(GaussKronrod());
  else integrator = GaussLegendre(Indices(2, static_cast<UnsignedInteger>(std::ceil(std::sqrt(1.0 * integrationNodesNumber_)))));
  // Integrate over the interval (-inf, y] of the conditioning Gamma distribution
  const Point upperCovariance(integrator.integrate(integrand, getRange()));
  UnsignedInteger index = 0;
  for (UnsignedInteger i = 0; i < conditionedDimension; ++i)
    for (UnsignedInteger j = i; j < dimension; ++j)
    {
      covariance_(i, j) = upperCovariance[index];
      ++index;
    }
  // The conditioning covariance can be reused as is
  const UnsignedInteger conditioningDimension = conditioningDistribution_.getDimension();
  Indices upper(conditioningDimension);
  upper.fill(conditionedDimension);
  const CovarianceMatrix conditioningCovariance(getMarginal(upper).getCovariance());
  for (UnsignedInteger i = 0; i < conditioningDimension; ++i)
    for (UnsignedInteger j = i; j < conditioningDimension; ++j)
      covariance_(conditionedDimension + i, conditionedDimension + j) = conditioningCovariance(i, j);
  isAlreadyComputedCovariance_ = true;
}

/* Method save() stores the object through the StorageManager */
void BayesDistribution::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute( "conditionedDistribution_", conditionedDistribution_ );
  adv.saveAttribute( "conditioningDistribution_", conditioningDistribution_ );
  adv.saveAttribute( "linkFunction_", linkFunction_ );
}

/* Method load() reloads the object from the StorageManager */
void BayesDistribution::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
  adv.loadAttribute( "conditionedDistribution_", conditionedDistribution_ );
  adv.loadAttribute( "conditioningDistribution_", conditioningDistribution_ );
  adv.loadAttribute( "linkFunction_", linkFunction_ );
  computeRange();
}


END_NAMESPACE_OPENTURNS

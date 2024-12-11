//                                               -*- C++ -*-
/**
 *  @brief The JointByConditioningDistribution distribution
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/JointByConditioningDistribution.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Uniform.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/IdentityFunction.hxx"
#include "openturns/SymbolicFunction.hxx"
#include "openturns/IntegrationAlgorithm.hxx"
#include "openturns/GaussKronrod.hxx"
#include "openturns/GaussLegendre.hxx"
#include "openturns/IteratedQuadrature.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(JointByConditioningDistribution)

static const Factory<JointByConditioningDistribution> Factory_JointByConditioningDistribution;


/* Default constructor */
JointByConditioningDistribution::JointByConditioningDistribution()
  : DistributionImplementation()
{
  const SymbolicFunction linkFunction(Description({"y0"}), Description({"y0", "y0 + 1"}));
  setConditionedAndConditioningDistributionsAndLinkFunction(Uniform(), Uniform(), linkFunction);
  setName("JointByConditioningDistribution");
}

/* Parameters constructor */
JointByConditioningDistribution::JointByConditioningDistribution(const Distribution & conditionedDistribution,
    const Distribution & conditioningDistribution,
    const Function & linkFunction0)
  : DistributionImplementation()
{
  if (!conditionedDistribution.isContinuous()) throw InvalidArgumentException(HERE) << "Error: the JointByConditioningDistribution is defined only for continuous conditioned distributions, here conditionedDistribution=" << conditionedDistribution;
  if (!conditioningDistribution.isContinuous()) throw InvalidArgumentException(HERE) << "Error: the JointByConditioningDistribution is defined only for continuous conditioned distributions, here conditioningDistribution=" << conditioningDistribution;

  Function linkFunction(linkFunction0);
  if (!linkFunction.getEvaluation().getImplementation()->isActualImplementation())
    linkFunction = IdentityFunction(conditioningDistribution.getDimension());

  setConditionedAndConditioningDistributionsAndLinkFunction(conditionedDistribution, conditioningDistribution, linkFunction);
  setName("JointByConditioningDistribution");
}

/* Comparison operator */
Bool JointByConditioningDistribution::operator ==(const JointByConditioningDistribution & other) const
{
  if (this == &other) return true;
  return (conditionedDistribution_ == other.conditionedDistribution_) && (conditioningDistribution_ == other.conditioningDistribution_) && (linkFunction_ == other.linkFunction_);
}

Bool JointByConditioningDistribution::equals(const DistributionImplementation & other) const
{
  const JointByConditioningDistribution* p_other = dynamic_cast<const JointByConditioningDistribution*>(&other);
  return p_other && (*this == *p_other);
}

/* Compute the numerical range of the distribution given the parameters values */
void JointByConditioningDistribution::computeRange()
{
  // First, the conditioning distribution
  Point lowerBound(conditioningDistribution_.getRange().getLowerBound());
  Interval::BoolCollection finiteLowerBound(conditioningDistribution_.getRange().getFiniteLowerBound());
  Point upperBound(conditioningDistribution_.getRange().getUpperBound());
  Interval::BoolCollection finiteUpperBound(conditioningDistribution_.getRange().getFiniteUpperBound());

  // Then, the conditioned distribution
  lowerBound.add(deconditioned_.getRange().getLowerBound());
  finiteLowerBound.add(deconditioned_.getRange().getFiniteLowerBound());
  upperBound.add(deconditioned_.getRange().getUpperBound());
  finiteUpperBound.add(deconditioned_.getRange().getFiniteUpperBound());

  setRange(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));
}

/* String converter */
String JointByConditioningDistribution::__repr__() const
{
  OSS oss(true);
  oss << "class=" << JointByConditioningDistribution::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " conditioned distribution=" << conditionedDistribution_
      << " conditioning distribution=" << conditioningDistribution_
      << " link function=" << linkFunction_;
  return oss;
}

String JointByConditioningDistribution::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(Y, X with X|Theta~" << conditionedDistribution_.getImplementation()->getClassName() << "(Theta), Theta=f(Y), f=" << linkFunction_.getEvaluation().__str__() << ", Y~" << conditioningDistribution_.__str__() << ")";
  return oss;
}

/* Virtual constructor */
JointByConditioningDistribution * JointByConditioningDistribution::clone() const
{
  return new JointByConditioningDistribution(*this);
}

/* Get one realization of the distribution */
Point JointByConditioningDistribution::getRealization() const
{
  Point yx(conditioningDistribution_.getRealization());
  Distribution deconditioned(conditionedDistribution_);
  deconditioned.setParameter(linkFunction_(yx));
  yx.add(deconditioned.getRealization());
  return yx;
}

/* Get the PDF of the distribution */
Scalar JointByConditioningDistribution::computePDF(const Point & point) const
{
  if (point.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << getDimension() << ", here dimension=" << point.getDimension();

  const UnsignedInteger conditionedDimension = conditionedDistribution_.getDimension();
  const UnsignedInteger conditioningDimension = conditioningDistribution_.getDimension();
  Point y(conditioningDimension);
  std::copy(point.begin(), point.begin() + conditioningDimension, y.begin());
  const Scalar conditioningPDF = conditioningDistribution_.computePDF(y);
  if (conditioningPDF == 0.0) return 0.0;
  Distribution deconditioned(conditionedDistribution_);
  const Point parameters(linkFunction_(y));
  deconditioned.setParameter(parameters);
  Point x(conditionedDimension);
  std::copy(point.begin() + conditioningDimension, point.end(), x.begin());
  const Scalar deconditionedPDF = deconditioned.computePDF(x);
  return deconditionedPDF * conditioningPDF;
}

/* Conditioned distribution accessor */
void JointByConditioningDistribution::setConditionedDistribution(const Distribution & conditionedDistribution)
{
  if (!conditionedDistribution.isContinuous()) throw InvalidArgumentException(HERE) << "Error: the JointByConditioningDistribution is defined only for continuous conditioned distributions, here conditionedDistribution=" << conditionedDistribution;
  if (conditionedDistribution != conditionedDistribution_)
    setConditionedAndConditioningDistributionsAndLinkFunction(conditionedDistribution, conditioningDistribution_, linkFunction_);
}

namespace
{
// Class used to compute the CDF of a JointByConditioning distribution
class JointByConditioningCDFKernel: public EvaluationImplementation
{
public:
  JointByConditioningCDFKernel(const Distribution & conditionedDistribution,
                               const Distribution & conditioningDistribution,
                               const Function & linkFunction,
                               const Point & x)
    : EvaluationImplementation()
    , conditionedDistribution_(conditionedDistribution)
    , conditioningDistribution_(conditioningDistribution)
    , linkFunction_(linkFunction)
    , x_(x)
  {
    // Nothing to do
  }

  JointByConditioningCDFKernel * clone() const
  {
    return new JointByConditioningCDFKernel(*this);
  }

  Point operator() (const Point & point) const
  {
    const Scalar pdfY = conditioningDistribution_.computePDF(point);
    if (pdfY == 0.0) return Point(1, 0.0);
    Distribution parameterized(conditionedDistribution_);
    const Point parameter(linkFunction_(point));
    parameterized.setParameter(parameter);
    const Scalar cdfX = parameterized.computeCDF(x_);
    return Point(1, pdfY * cdfX);
  }

  UnsignedInteger getInputDimension() const
  {
    return conditioningDistribution_.getDimension();
  }

  UnsignedInteger getOutputDimension() const
  {
    return 1;
  }

  Description getInputDescription() const
  {
    return conditioningDistribution_.getDescription();
  }

  Description getOutputDescription() const
  {
    return Description(1, "JointByConditioningCDFKernel");
  }

private:
  const Distribution & conditionedDistribution_;
  const Distribution & conditioningDistribution_;
  const Function & linkFunction_;
  const Point & x_;
};  // class JointByConditioningCDFKernel
} // anonymous namespace

/* Get the CDF of the distribution */
Scalar JointByConditioningDistribution::computeCDF(const Point & point) const
{
  if (point.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << getDimension() << ", here dimension=" << point.getDimension();
  Point y(conditioningDistribution_.getDimension());
  std::copy(point.begin(), point.begin() + y.getSize(), y.begin());
  Point x(conditionedDistribution_.getDimension());
  std::copy(point.begin() + y.getSize(), point.end(), x.begin());
  Point lowerY(conditioningDistribution_.getRange().getLowerBound());
  const Function kernel(JointByConditioningCDFKernel(conditionedDistribution_, conditioningDistribution_, linkFunction_, x));
  const Scalar cdf = IteratedQuadrature().integrate(kernel, Interval(lowerY, y))[0];
  return SpecFunc::Clip01(cdf);
}

Distribution JointByConditioningDistribution::getConditionedDistribution() const
{
  return conditionedDistribution_;
}


/* Conditioning distribution accessor */
void JointByConditioningDistribution::setConditioningDistribution(const Distribution & conditioningDistribution)
{
  if (!conditioningDistribution.isContinuous()) throw InvalidArgumentException(HERE) << "Error: the JointByConditioningDistribution is defined only for continuous conditioned distributions, here conditioningDistribution=" << conditioningDistribution;
  if (conditioningDistribution != conditioningDistribution_)
    setConditionedAndConditioningDistributionsAndLinkFunction(conditionedDistribution_, conditioningDistribution, linkFunction_);
}

Distribution JointByConditioningDistribution::getConditioningDistribution() const
{
  return conditioningDistribution_;
}

/* Link function accessor */
void JointByConditioningDistribution::setLinkFunction(const Function & linkFunction)
{
  if (!(linkFunction == linkFunction_))
    setConditionedAndConditioningDistributionsAndLinkFunction(conditionedDistribution_, conditioningDistribution_, linkFunction);
}

Function JointByConditioningDistribution::getLinkFunction() const
{
  return linkFunction_;
}

/* Method to set simultaneously the conditioning distribution, the conditioned distribution and the link function */
void JointByConditioningDistribution::setConditionedAndConditioningDistributionsAndLinkFunction(const Distribution & conditionedDistribution,
    const Distribution & conditioningDistribution,
    const Function & linkFunction)
{
  const UnsignedInteger conditioningDimension = conditioningDistribution.getDimension();
  const UnsignedInteger conditionedParametersDimension = conditionedDistribution.getParameterDimension();
  // We must check that the conditioning distribution has the same dimension as the input dimension of the link function and that the conditioning distribution has the same dimension as the input dimension of the link function
  if (conditionedParametersDimension != linkFunction.getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: expected a link function with output dimension equal to the number of parameters of the conditioned distribution.";
  if (conditioningDimension != linkFunction.getInputDimension())
    throw InvalidArgumentException(HERE) << "Error: expected a link function with input dimension equal to the conditioning distribution dimension.";
  conditionedDistribution_ = conditionedDistribution;
  conditioningDistribution_ = conditioningDistribution;
  linkFunction_ = linkFunction;
  setDimension(conditioningDimension + conditionedDistribution.getDimension());
  deconditioned_ = DeconditionedDistribution(conditionedDistribution, conditioningDistribution, linkFunction);
  computeRange();

  Description description(conditioningDistribution.getDescription());
  description.add(conditionedDistribution.getDescription());

  // avoid description warning with identical entries
  std::sort(description.begin(), description.end());
  if (std::unique(description.begin(), description.end()) != description.end())
  {
    description = Description::BuildDefault(conditioningDistribution.getDimension(), "Y");
    description.add(Description::BuildDefault(conditionedDistribution.getDimension(), "X"));
  }

  setDescription(description);
  const Bool flag1 = linkFunction_.getEvaluation().getImplementation()->isParallel();
  const Bool flag2 = conditioningDistribution_.getImplementation()->isParallel();
  const Bool flag3 = conditionedDistribution_.getImplementation()->isParallel();
  isParallel_ = flag1 && flag2 && flag3;
}

/* Get the i-th marginal distribution */
Distribution JointByConditioningDistribution::getMarginal(const UnsignedInteger i) const
{
  if (i >= getDimension()) throw InvalidArgumentException(HERE) << "The index of a marginal distribution must be in the range [0, dim-1]";
  // Special case for dimension 1
  if (getDimension() == 1) return clone();
  // General case
  // If the index is in the conditioning part
  const UnsignedInteger conditioningDimension = conditioningDistribution_.getDimension();
  if (i < conditioningDimension) return conditioningDistribution_.getMarginal(i);
  return deconditioned_.getMarginal(i - conditioningDimension);
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
Distribution JointByConditioningDistribution::getMarginal(const Indices & indices) const
{
  const UnsignedInteger dimension = getDimension();
  if (!indices.check(dimension))
    throw InvalidArgumentException(HERE) << "The indices of a marginal distribution must be in the range [0, dim-1] and must be different";
  // Special case for dimension 1
  if (dimension == 1) return clone();
  // General case
  // If the indices are in the conditioning part
  const UnsignedInteger conditioningDimension = conditioningDistribution_.getDimension();
  if (indices.check(conditioningDimension)) return conditioningDistribution_.getMarginal(indices);
  // If the indices are in the conditioned part
  Indices conditionedIndices(0);
  const UnsignedInteger size = indices.getSize();
  for (UnsignedInteger i = 0; i < size; ++i)
    if (indices[i] >= conditioningDimension)
      conditionedIndices.add(indices[i] - conditioningDimension);
  if (conditionedIndices.getSize() == size)
    return deconditioned_.getMarginal(conditionedIndices);
  return DistributionImplementation::getMarginal(indices);
} // getMarginal(Indices)

/* Parameters value and description accessor */
Point JointByConditioningDistribution::getParameter() const
{
  Point parameter(linkFunction_.getParameter());
  parameter.add(conditioningDistribution_.getParameter());
  return parameter;
} // getParameter

void JointByConditioningDistribution::setParameter(const Point & parameter)
{
  if (parameter.getSize() != getParameter().getSize())
    throw InvalidArgumentException(HERE) << "Error: expected " << getParameter().getSize() << " values, got " << parameter.getSize();
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
  *this = JointByConditioningDistribution(conditionedDistribution_, conditioningDistribution_, linkFunction_);
  setWeight(w);
} // setParameter

/* Parameters value and description accessor */
Description JointByConditioningDistribution::getParameterDescription() const
{
  Description parameterDescription(linkFunction_.getParameterDescription());
  parameterDescription.add(conditioningDistribution_.getParameterDescription());
  return parameterDescription;
} // getParameterDescription

/* Compute the mean of the distribution */
void JointByConditioningDistribution::computeMean() const
{
  const UnsignedInteger conditioningDimension = conditioningDistribution_.getDimension();
  Indices lower(conditioningDimension);
  lower.fill();
  mean_ = getMarginal(lower).getMean();
  const UnsignedInteger conditionedDimension = conditionedDistribution_.getDimension();
  Indices upper(conditionedDimension);
  upper.fill(conditioningDimension);
  mean_.add(getMarginal(upper).getMean());
  isAlreadyComputedMean_ = true;
}

namespace JointByConditioningDistributionFunctions
{

class KernelCovariance: public EvaluationImplementation
{
public:
  KernelCovariance(const JointByConditioningDistribution & distribution)
    : EvaluationImplementation()
    , distribution_(distribution)
    , dimension_(distribution.getDimension())
    , conditioningDimension_(distribution.getConditioningDistribution().getDimension())
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
    for (UnsignedInteger i = conditioningDimension_; i < dimension_; ++ i)
    {
      const Scalar deltaI(point[i] - mu_[i]);
      for (UnsignedInteger j = 0; j <= i; ++ j)
      {
        const Scalar deltaJ(point[j] - mu_[j]);
        value[index] = pdf * deltaI * deltaJ;
        ++ index;
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
  const JointByConditioningDistribution & distribution_;
  const UnsignedInteger dimension_;
  const UnsignedInteger conditioningDimension_;
  const UnsignedInteger conditionedDimension_;
  const UnsignedInteger outputDimension_;
  const Point mu_;
}; // KernelCovariance

} // namespace JointByConditioningDistributionFunctions

/* Compute the covariance of the distribution */
void JointByConditioningDistribution::computeCovariance() const
{
  const UnsignedInteger dimension = getDimension();
  covariance_ = CovarianceMatrix(dimension);
  const Function integrand(JointByConditioningDistributionFunctions::KernelCovariance(*this));
  const Bool useAdaptiveAlgorithm = ResourceMap::GetAsBool("Distribution-UseCovarianceAdaptiveAlgorithm");
  IntegrationAlgorithm integrator;
  if (useAdaptiveAlgorithm) integrator = IteratedQuadrature(GaussKronrod());
  else integrator = GaussLegendre(Indices(2, static_cast<UnsignedInteger>(std::ceil(std::sqrt(1.0 * integrationNodesNumber_)))));

  // The conditioning covariance can be reused as is
  const UnsignedInteger conditioningDimension = conditioningDistribution_.getDimension();
  const CovarianceMatrix conditioningCovariance(conditioningDistribution_.getCovariance());
  for (UnsignedInteger i = 0; i < conditioningDimension; ++ i)
    for (UnsignedInteger j = 0; j <= i; ++ j)
      covariance_(i, j) = conditioningCovariance(i, j);

  // Complete the lower part
  const Point upperCovariance(integrator.integrate(integrand, getRange()));
  UnsignedInteger index = 0;
  for (UnsignedInteger i = conditioningDimension; i < dimension; ++ i)
    for (UnsignedInteger j = 0; j <= i; ++ j)
    {
      covariance_(i, j) = upperCovariance[index];
      ++ index;
    }
  isAlreadyComputedCovariance_ = true;
}

/* Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
Scalar JointByConditioningDistribution::computeConditionalPDF(const Scalar x,
    const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional PDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  // Special case for a conditioning only in the conditioning part
  const UnsignedInteger conditioningDistributionDimension = conditioningDistribution_.getDimension();
  if ((conditioningDimension < conditioningDistributionDimension))
    return conditioningDistribution_.computeConditionalPDF(x, y);
  // The conditioning part is fully conditioned, let's evaluate the link function
  Point fixedConditioningPart(conditioningDistributionDimension);
  std::copy(y.begin(), y.begin() + conditioningDistributionDimension, fixedConditioningPart.begin());
  Distribution conditioned(conditionedDistribution_);
  conditioned.setParameter(linkFunction_(fixedConditioningPart));
  Point fixedConditionedPart(conditioningDistributionDimension - conditioningDistributionDimension);
  if (fixedConditionedPart.getDimension() > 0)
    std::copy(y.begin() + conditioningDistributionDimension, y.end(), fixedConditionedPart.begin());
  return conditioned.computeConditionalPDF(x, fixedConditionedPart);
}

/* Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
Point JointByConditioningDistribution::computeSequentialConditionalPDF(const Point & x) const
{
  if (x.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << dimension_ << ", got dimension=" << x.getDimension();
  const UnsignedInteger conditioningDistributionDimension = conditioningDistribution_.getDimension();
  Point result(dimension_);
  Point conditioningArgument(conditioningDistributionDimension);
  std::copy(x.begin(), x.begin() + conditioningDistributionDimension, conditioningArgument.begin());
  const Point conditioningConditionalPDF(conditioningDistribution_.computeSequentialConditionalPDF(conditioningArgument));
  std::copy(conditioningConditionalPDF.begin(), conditioningConditionalPDF.begin() + conditioningDistributionDimension, result.begin());
  Distribution conditioned(conditionedDistribution_);
  conditioned.setParameter(linkFunction_(conditioningArgument));
  Point conditionedArgument(conditioned.getDimension());
  std::copy(x.begin() + conditioningDistributionDimension, x.end(), conditionedArgument.begin());
  const Point conditionedConditionalPDF(conditioned.computeSequentialConditionalPDF(conditionedArgument));
  std::copy(conditionedConditionalPDF.begin(), conditionedConditionalPDF.end(), result.begin() + conditioningDistributionDimension);
  return result;
}

/* Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
Scalar JointByConditioningDistribution::computeConditionalCDF(const Scalar x,
    const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional CDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  // Special case for a conditioning only in the conditioning part
  const UnsignedInteger conditioningDistributionDimension = conditioningDistribution_.getDimension();
  if ((conditioningDimension < conditioningDistributionDimension))
    return conditioningDistribution_.computeConditionalCDF(x, y);
  // The conditioning part is fully conditioned, let's evaluate the link function
  Point fixedConditioningPart(conditioningDistributionDimension);
  std::copy(y.begin(), y.begin() + conditioningDistributionDimension, fixedConditioningPart.begin());
  Distribution conditioned(conditionedDistribution_);
  conditioned.setParameter(linkFunction_(fixedConditioningPart));
  Point fixedConditionedPart(conditioningDimension - conditioningDistributionDimension);
  if (fixedConditionedPart.getDimension() > 0)
    std::copy(y.begin() + conditioningDistributionDimension, y.end(), fixedConditionedPart.begin());
  return conditioned.computeConditionalCDF(x, fixedConditionedPart);
}

/* Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
Point JointByConditioningDistribution::computeSequentialConditionalCDF(const Point & x) const
{
  if (x.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << dimension_ << ", got dimension=" << x.getDimension();
  const UnsignedInteger conditioningDistributionDimension = conditioningDistribution_.getDimension();
  Point result(dimension_);
  Point conditioningArgument(conditioningDistributionDimension);
  std::copy(x.begin(), x.begin() + conditioningDistributionDimension, conditioningArgument.begin());
  const Point conditioningConditionalCDF(conditioningDistribution_.computeSequentialConditionalCDF(conditioningArgument));
  std::copy(conditioningConditionalCDF.begin(), conditioningConditionalCDF.begin() + conditioningDistributionDimension, result.begin());
  Distribution conditioned(conditionedDistribution_);
  conditioned.setParameter(linkFunction_(conditioningArgument));
  Point conditionedArgument(conditioned.getDimension());
  std::copy(x.begin() + conditioningDistributionDimension, x.end(), conditionedArgument.begin());
  const Point conditionedConditionalCDF(conditioned.computeSequentialConditionalCDF(conditionedArgument));
  std::copy(conditionedConditionalCDF.begin(), conditionedConditionalCDF.end(), result.begin() + conditioningDistributionDimension);
  return result;
}

/* Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
Scalar JointByConditioningDistribution::computeConditionalQuantile(const Scalar q,
    const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile with a conditioning point of dimension greater or equal to the distribution dimension.";
  if ((q < 0.0) || (q > 1.0)) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile for a probability level outside of [0, 1]";
  // Special case for a conditioning only in the conditioning part
  const UnsignedInteger conditioningDistributionDimension = conditioningDistribution_.getDimension();
  if ((conditioningDimension < conditioningDistributionDimension))
    return conditioningDistribution_.computeConditionalQuantile(q, y);
  // The conditioning part is fully conditioned, let's evaluate the link function
  Point fixedConditioningPart(conditioningDistributionDimension);
  std::copy(y.begin(), y.begin() + conditioningDistributionDimension, fixedConditioningPart.begin());
  Distribution conditioned(conditionedDistribution_);
  conditioned.setParameter(linkFunction_(fixedConditioningPart));
  Point fixedConditionedPart(conditioningDimension - conditioningDistributionDimension);
  if (fixedConditionedPart.getDimension() > 0)
    std::copy(y.begin() + conditioningDistributionDimension, y.end(), fixedConditionedPart.begin());
  return conditioned.computeConditionalQuantile(q, fixedConditionedPart);
}

/* Compute the quantile of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
Point JointByConditioningDistribution::computeSequentialConditionalQuantile(const Point & q) const
{
  if (q.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << dimension_ << ", got dimension=" << q.getDimension();
  const UnsignedInteger conditioningDistributionDimension = conditioningDistribution_.getDimension();
  Point result(dimension_);
  Point conditioningArgument(conditioningDistributionDimension);
  std::copy(q.begin(), q.begin() + conditioningDistributionDimension, conditioningArgument.begin());
  const Point conditioningConditionalQuantile(conditioningDistribution_.computeSequentialConditionalQuantile(conditioningArgument));
  std::copy(conditioningConditionalQuantile.begin(), conditioningConditionalQuantile.begin() + conditioningDistributionDimension, result.begin());
  Distribution conditioned(conditionedDistribution_);
  conditioned.setParameter(linkFunction_(conditioningConditionalQuantile));
  Point conditionedArgument(conditioned.getDimension());
  std::copy(q.begin() + conditioningDistributionDimension, q.end(), conditionedArgument.begin());
  const Point conditionedConditionalQuantile(conditioned.computeSequentialConditionalQuantile(conditionedArgument));
  std::copy(conditionedConditionalQuantile.begin(), conditionedConditionalQuantile.end(), result.begin() + conditioningDistributionDimension);
  return result;
}

/* Method save() stores the object through the StorageManager */
void JointByConditioningDistribution::save(Advocate & adv) const
{
  DistributionImplementation::save(adv);
  adv.saveAttribute( "conditionedDistribution_", conditionedDistribution_ );
  adv.saveAttribute( "conditioningDistribution_", conditioningDistribution_ );
  adv.saveAttribute( "linkFunction_", linkFunction_ );
}

/* Method load() reloads the object from the StorageManager */
void JointByConditioningDistribution::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  adv.loadAttribute( "conditionedDistribution_", conditionedDistribution_ );
  adv.loadAttribute( "conditioningDistribution_", conditioningDistribution_ );
  adv.loadAttribute( "linkFunction_", linkFunction_ );
  setConditionedAndConditioningDistributionsAndLinkFunction(conditionedDistribution_, conditioningDistribution_, linkFunction_);
}

END_NAMESPACE_OPENTURNS

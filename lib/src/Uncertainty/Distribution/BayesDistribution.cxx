//                                               -*- C++ -*-
/**
 *  @brief The BayesDistribution distribution
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/BayesDistribution.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Uniform.hxx"
#include "openturns/ConditionalDistribution.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/SymbolicFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(BayesDistribution);

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
  setConditionedAndConditioningDistributionsAndLinkFunction(conditionedDistribution, conditioningDistribution, linkFunction);
  setName("BayesDistribution");
  isParallel_ = false;
}

BayesDistribution::BayesDistribution(const Distribution & conditionedDistribution,
                                     const Distribution & conditioningDistribution)
  : ContinuousDistribution()
  , conditionedDistribution_(conditionedDistribution)
  , conditioningDistribution_(conditioningDistribution)
  , linkFunction_(SymbolicFunction(Description::BuildDefault(conditioningDistribution.getDimension(), "x"), Description::BuildDefault(conditioningDistribution.getDimension(), "x")))
{
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
  const NumericalPoint lowerBoundConditioning(conditioningDistribution_.getRange().getLowerBound());
  const Interval::BoolCollection finiteLowerBoundConditioning(conditioningDistribution_.getRange().getFiniteLowerBound());
  const NumericalPoint upperBoundConditioning(conditioningDistribution_.getRange().getUpperBound());
  const Interval::BoolCollection finiteUpperBoundConditioning(conditioningDistribution_.getRange().getFiniteUpperBound());
  // Then, the conditioned distribution
  const ConditionalDistribution deconditioned(conditionedDistribution_, conditioningDistribution_, linkFunction_);
  NumericalPoint lowerBound(deconditioned.getRange().getLowerBound());
  Interval::BoolCollection finiteLowerBound(deconditioned.getRange().getFiniteLowerBound());
  NumericalPoint upperBound(deconditioned.getRange().getUpperBound());
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

String BayesDistribution::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << getClassName() << "(X, Y with X|Theta~" << conditionedDistribution_.getImplementation()->getClassName() << "(Theta), Theta=f(Y), f=" << linkFunction_.getEvaluation()->__str__() << ", Y~" << conditioningDistribution_.__str__() << ")";
  return oss;
}

/* Virtual constructor */
BayesDistribution * BayesDistribution::clone() const
{
  return new BayesDistribution(*this);
}

/* Get one realization of the distribution */
NumericalPoint BayesDistribution::getRealization() const
{
  const NumericalPoint y(conditioningDistribution_.getRealization());
  Distribution deconditioned(conditionedDistribution_);
  deconditioned.setParameter(linkFunction_(y));
  NumericalPoint x(deconditioned.getRealization());
  x.add(y);
  return x;
}

/* Get the PDF of the distribution */
NumericalScalar BayesDistribution::computePDF(const NumericalPoint & point) const
{
  if (point.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << getDimension() << ", here dimension=" << point.getDimension();

  const UnsignedInteger conditionedDimension = conditionedDistribution_.getDimension();
  const UnsignedInteger conditioningDimension = conditioningDistribution_.getDimension();
  NumericalPoint y(conditioningDimension);
  std::copy(point.begin() + conditionedDimension, point.end(), y.begin());
  const NumericalScalar conditioningPDF = conditioningDistribution_.computePDF(y);
  if (conditioningPDF == 0.0) return 0.0;
  Distribution deconditioned(conditionedDistribution_);
  const NumericalPoint parameters(linkFunction_(y));
  deconditioned.setParameter(parameters);
  NumericalPoint x(conditionedDimension);
  std::copy(point.begin(), point.begin() + conditionedDimension, x.begin());
  const NumericalScalar deconditionedPDF = deconditioned.computePDF(x);
  return deconditionedPDF * conditioningPDF;
}

/* Bayes distribution accessor */
void BayesDistribution::setConditionedDistribution(const Distribution & conditionedDistribution)
{
  if (conditionedDistribution != conditionedDistribution_) setConditionedAndConditioningDistributionsAndLinkFunction(conditionedDistribution, conditioningDistribution_, linkFunction_);
}

Distribution BayesDistribution::getConditionedDistribution() const
{
  return conditionedDistribution_;
}


/* Conditioning distribution accessor */
void BayesDistribution::setConditioningDistribution(const Distribution & conditioningDistribution)
{
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
BayesDistribution::Implementation BayesDistribution::getMarginal(const UnsignedInteger i) const
{
  if (i >= getDimension()) throw InvalidArgumentException(HERE) << "The index of a marginal distribution must be in the range [0, dim-1]";
  // Special case for dimension 1
  if (getDimension() == 1) return clone();
  // General case
  // If the index is in the conditioned part
  const UnsignedInteger conditionedDimension = conditionedDistribution_.getDimension();
  if (i < conditionedDimension) return ConditionalDistribution(conditionedDistribution_, conditioningDistribution_, linkFunction_).getMarginal(i);
  return conditioningDistribution_.getImplementation()->getMarginal(i - conditionedDimension);
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
BayesDistribution::Implementation BayesDistribution::getMarginal(const Indices & indices) const
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
    if (i >= conditionedDimension)
      conditioningIndices.add(i - conditionedDimension);
  if (conditioningIndices.getSize() == size) return conditioningDistribution_.getImplementation()->getMarginal(conditioningIndices);
  throw NotYetImplementedException(HERE) << "In BayesDistribution::getMarginal(const Indices & indices) const";
} // getMarginal(Indices)

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

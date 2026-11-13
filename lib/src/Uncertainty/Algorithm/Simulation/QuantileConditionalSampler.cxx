//                                               -*- C++ -*-
/**
 *  @brief Conditional sampler shared by the threshold quantile algorithms
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/QuantileConditionalSampler.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/Normal.hxx"
#include "openturns/IdentityFunction.hxx"
#include "openturns/ComposedFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

/* Default constructor */
QuantileConditionalSampler::QuantileConditionalSampler()
  : standardFunction_(IdentityFunction(1))
{
  // Nothing to do
}

/* Constructor from the scalar output */
QuantileConditionalSampler::QuantileConditionalSampler(const RandomVector & output,
                                                       const Scalar proposalRange,
                                                       const UnsignedInteger burnIn)
  : standardFunction_()
  , dimension_(1)
  , proposalRange_(proposalRange)
  , burnIn_(burnIn)
{
  const CompositeRandomVector composite(buildCompositeOutput(output));
  const Distribution distribution(composite.getAntecedent().getDistribution());
  dimension_ = distribution.getDimension();
  if (composite.getFunction().getOutputDimension() != 1)
    throw InvalidArgumentException(HERE)
        << "The conditional sampler requires a scalar output, but the output dimension is "
        << composite.getFunction().getOutputDimension();
  const Function inverse(distribution.getInverseIsoProbabilisticTransformation());
  standardFunction_ = ComposedFunction(composite.getFunction(), inverse);
  setProposalRange(proposalRange);
  setBurnIn(burnIn);
}

/* Build a composite representation of a scalar output */
CompositeRandomVector QuantileConditionalSampler::buildCompositeOutput(const RandomVector & output)
{
  if (output.isComposite())
    return CompositeRandomVector(output.getFunction(), output.getAntecedent());
  Distribution distribution;
  try
  {
    distribution = output.getDistribution();
  }
  catch (const Exception &)
  {
    throw InvalidArgumentException(HERE)
        << "The conditional sampler requires a composite random vector or a random vector "
        << "whose distribution can be retrieved";
  }
  return CompositeRandomVector(IdentityFunction(distribution.getDimension()),
                               RandomVector(distribution));
}

/* Standard-space population */
Sample QuantileConditionalSampler::generatePopulation(const UnsignedInteger size) const
{
  if (size == 0)
    throw InvalidArgumentException(HERE) << "The population size must be positive";
  return Normal(dimension_).getSample(size);
}

/* Evaluate the output in standard space */
Sample QuantileConditionalSampler::evaluate(const Sample & standardSample) const
{
  if (standardSample.getDimension() != dimension_)
    throw InvalidArgumentException(HERE)
        << "The standard sample dimension (" << standardSample.getDimension()
        << ") does not match the input dimension (" << dimension_ << ")";
  const Sample outputSample(standardFunction_(standardSample));
  if (outputSample.getDimension() != 1)
    throw InternalException(HERE)
        << "The standard output dimension is " << outputSample.getDimension()
        << " instead of 1";
  evaluationCount_ += standardSample.getSize();
  return outputSample;
}

Scalar QuantileConditionalSampler::evaluate(const Point & standardPoint) const
{
  if (standardPoint.getDimension() != dimension_)
    throw InvalidArgumentException(HERE)
        << "The standard point dimension (" << standardPoint.getDimension()
        << ") does not match the input dimension (" << dimension_ << ")";
  const Point outputPoint(standardFunction_(standardPoint));
  if (outputPoint.getSize() != 1)
    throw InternalException(HERE)
        << "The standard output dimension is " << outputPoint.getSize()
        << " instead of 1";
  ++evaluationCount_;
  return outputPoint[0];
}

/* One constrained Metropolis-Hastings transition */
Point QuantileConditionalSampler::transition(const Point & start,
                                             const ComparisonOperator & op,
                                             const Scalar threshold) const
{
  Point current(start);
  const Scalar halfRange = 0.5 * proposalRange_;
  for (UnsignedInteger k = 0; k < dimension_; ++k)
  {
    const Scalar oldValue = current[k];
    const Scalar newValue = oldValue + (2.0 * RandomGenerator::Generate() - 1.0) * halfRange;
    // The target is the standard normal distribution.  The proposal
    // is symmetric, so the acceptance ratio is the density ratio.
    const Scalar logRatio = 0.5 * (oldValue * oldValue - newValue * newValue);
    if (std::log(RandomGenerator::Generate()) < logRatio)
      current[k] = newValue;
  }

  ++proposedTransitions_;
  const Scalar value = evaluate(current);
  if (op(value, threshold))
  {
    ++acceptedTransitions_;
    return current;
  }
  return start;
}

/* Draw one point from the conditional distribution */
Point QuantileConditionalSampler::sample(const Point & start,
                                         const ComparisonOperator & op,
                                         const Scalar threshold) const
{
  if (start.getDimension() != dimension_)
    throw InvalidArgumentException(HERE)
        << "The starting point dimension (" << start.getDimension()
        << ") does not match the input dimension (" << dimension_ << ")";
  Point current(start);
  for (UnsignedInteger i = 0; i < burnIn_; ++i)
    current = transition(current, op, threshold);
  return current;
}

/* Dimension accessor */
UnsignedInteger QuantileConditionalSampler::getDimension() const
{
  return dimension_;
}

/* Proposal range accessor */
Scalar QuantileConditionalSampler::getProposalRange() const
{
  return proposalRange_;
}

void QuantileConditionalSampler::setProposalRange(const Scalar proposalRange)
{
  if (!(proposalRange > 0.0) || !std::isfinite(proposalRange))
    throw InvalidArgumentException(HERE)
        << "The proposal range must be finite and positive, but proposalRange = " << proposalRange;
  proposalRange_ = proposalRange;
}

/* Burn-in accessor */
UnsignedInteger QuantileConditionalSampler::getBurnIn() const
{
  return burnIn_;
}

void QuantileConditionalSampler::setBurnIn(const UnsignedInteger burnIn)
{
  if (burnIn == 0)
    throw InvalidArgumentException(HERE) << "The burn-in must be positive";
  burnIn_ = burnIn;
}

/* MCMC diagnostics */
void QuantileConditionalSampler::resetDiagnostics() const
{
  proposedTransitions_ = 0;
  acceptedTransitions_ = 0;
  evaluationCount_ = 0;
}

Scalar QuantileConditionalSampler::getAcceptanceRate() const
{
  if (proposedTransitions_ == 0)
    return 0.0;
  return 1.0 * acceptedTransitions_ / (1.0 * proposedTransitions_);
}

UnsignedInteger QuantileConditionalSampler::getTransitionCount() const
{
  return proposedTransitions_;
}

UnsignedInteger QuantileConditionalSampler::getAcceptedTransitionCount() const
{
  return acceptedTransitions_;
}

UnsignedInteger QuantileConditionalSampler::getEvaluationCount() const
{
  return evaluationCount_;
}

/* String converter */
String QuantileConditionalSampler::__repr__() const
{
  OSS oss;
  oss << "class=QuantileConditionalSampler"
      << " dimension=" << dimension_
      << " proposalRange=" << proposalRange_
      << " burnIn=" << burnIn_;
  return oss;
}

END_NAMESPACE_OPENTURNS

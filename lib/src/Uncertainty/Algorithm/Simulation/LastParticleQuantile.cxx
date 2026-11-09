//                                               -*- C++ -*-
/**
 *  @brief Last-particle quantile estimator
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
#include <algorithm>
#include <limits>

#include "openturns/LastParticleQuantile.hxx"
#include "openturns/QuantileConditionalSampler.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/Less.hxx"
#include "openturns/Greater.hxx"
#include "openturns/Indices.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(LastParticleQuantile)

static const Factory<LastParticleQuantile> Factory_LastParticleQuantile;

/* Default constructor */
LastParticleQuantile::LastParticleQuantile()
  : QuantileSimulationImplementation()
{
  sampleSize_ = particleCount_;
}

/* Constructor with parameters */
LastParticleQuantile::LastParticleQuantile(const RandomVector & output,
                                           const Scalar alpha,
                                           const Scalar beta)
  : QuantileSimulationImplementation(output, alpha, beta)
{
  sampleSize_ = particleCount_;
}

/* Virtual constructor */
LastParticleQuantile * LastParticleQuantile::clone() const
{
  return new LastParticleQuantile(*this);
}

/* Particle count accessor */
void LastParticleQuantile::setParticleCount(const UnsignedInteger particleCount)
{
  if (particleCount < 10)
    throw InvalidArgumentException(HERE)
        << "The last-particle algorithm requires at least 10 particles, but particleCount = "
        << particleCount;
  particleCount_ = particleCount;
  sampleSize_ = particleCount;
}

UnsignedInteger LastParticleQuantile::getParticleCount() const
{
  return particleCount_;
}

/* Sample size accessor */
void LastParticleQuantile::setSampleSize(const UnsignedInteger sampleSize)
{
  setParticleCount(sampleSize);
  QuantileSimulationImplementation::setSampleSize(sampleSize);
}

/* Burn-in accessor */
void LastParticleQuantile::setBurnIn(const UnsignedInteger burnIn)
{
  if (burnIn == 0)
    throw InvalidArgumentException(HERE) << "The burn-in must be positive";
  burnIn_ = burnIn;
}

UnsignedInteger LastParticleQuantile::getBurnIn() const
{
  return burnIn_;
}

/* Proposal range accessor */
void LastParticleQuantile::setProposalRange(const Scalar proposalRange)
{
  if (!(proposalRange > 0.0) || !std::isfinite(proposalRange))
    throw InvalidArgumentException(HERE)
        << "The proposal range must be finite and positive, but proposalRange = " << proposalRange;
  proposalRange_ = proposalRange;
}

Scalar LastParticleQuantile::getProposalRange() const
{
  return proposalRange_;
}

/* Run the last-particle process */
QuantileSimulationResult LastParticleQuantile::runFixedSample()
{
  const UnsignedInteger particleCount = particleCount_;
  if (particleCount < 10)
    throw InvalidArgumentException(HERE)
        << "The last-particle algorithm requires at least 10 particles, but particleCount = "
        << particleCount;

  const Bool lowerTail = alpha_ <= 0.5;
  const Scalar tailProbability = lowerTail ? alpha_ : 1.0 - alpha_;
  const ComparisonOperator op = lowerTail ? ComparisonOperator(Less()) : ComparisonOperator(Greater());

  // Poisson move count and rank interval.  The first crossing move is
  // K = P + 1, where P is Poisson with mean -N log(p).  The interval
  // [t_L, t_U] covers the quantile only when L < K <= U, i.e. when
  // L <= P <= U - 1.  Its coverage is F(U - 1) - F(L - 1), where F is
  // the Poisson distribution function.  The lower rank is the
  // (1-beta)/2 Poisson quantile and the upper rank is one above the
  // (1+beta)/2 Poisson quantile: the shift by one in U is necessary
  // because F(Q(t) - 1) can fall below t.  A zero lower quantile would
  // put L at the nonexistent move zero; reject that configuration
  // instead of truncating the rank, which would silently undercover.
  const Scalar expectedMoves = -1.0 * particleCount * std::log(tailProbability);
  const UnsignedInteger moveCountEstimate = std::max<UnsignedInteger>(1, static_cast<UnsignedInteger>(std::ceil(expectedMoves)));
  const UnsignedInteger lowerRank = static_cast<UnsignedInteger>(DistFunc::qPoisson(expectedMoves, 0.5 * (1.0 - beta_)));
  if (lowerRank == 0)
    throw InvalidArgumentException(HERE)
        << "The confidence level is too high for the particle count: the lower Poisson rank is zero, "
        << "so the exact lower endpoint is infinite. "
        << "Increase the particle count or decrease the confidence level";
  const UnsignedInteger upperRank = std::max<UnsignedInteger>(lowerRank, static_cast<UnsignedInteger>(DistFunc::qPoisson(expectedMoves, 0.5 * (1.0 + beta_)) + 1.0));
  const UnsignedInteger moveCount = std::max(upperRank, moveCountEstimate);
  const Scalar z = DistFunc::qNormal(0.5 * (1.0 + beta_));

  QuantileConditionalSampler sampler(output_, proposalRange_, burnIn_);
  sampler.resetDiagnostics();
  Sample standardSample(sampler.generatePopulation(particleCount));
  Sample outputSample(sampler.evaluate(standardSample));

  Point thresholds(moveCount);
  for (UnsignedInteger move = 0; move < moveCount; ++move)
  {
    // Locate the extreme particle.
    UnsignedInteger extremeIndex = 0;
    Scalar extremeValue = outputSample(0, 0);
    for (UnsignedInteger i = 1; i < particleCount; ++i)
    {
      const Scalar value = outputSample(i, 0);
      if (lowerTail ? (value > extremeValue) : (value < extremeValue))
      {
        extremeIndex = i;
        extremeValue = value;
      }
    }
    thresholds[move] = extremeValue;

    // The replacement is drawn from a particle that satisfies the
    // strict conditional event.  If no such particle exists, all the
    // outputs are tied and the conditional distribution is degenerate.
    Indices survivors;
    for (UnsignedInteger i = 0; i < particleCount; ++i)
      if (op(outputSample(i, 0), extremeValue))
        survivors.add(i);
    if (survivors.getSize() == 0)
      throw NotDefinedException(HERE)
          << "No particle satisfies the conditional event at move " << (move + 1)
          << " with threshold " << extremeValue
          << ". Increase the particle count or use a continuous output.";

    const UnsignedInteger seedIndex = survivors[RandomGenerator::IntegerGenerate(survivors.getSize())];
    const Point replacement(sampler.sample(standardSample[seedIndex], op, extremeValue));
    standardSample[extremeIndex] = replacement;
    outputSample(extremeIndex, 0) = sampler.evaluate(replacement);
  }

  const Scalar estimate = (moveCountEstimate >= 2)
                          ? 0.5 * (thresholds[moveCountEstimate - 2] + thresholds[moveCountEstimate - 1])
                          : thresholds[moveCountEstimate - 1];
  const Scalar lowerBound = lowerTail ? thresholds[upperRank - 1] : thresholds[lowerRank - 1];
  const Scalar upperBound = lowerTail ? thresholds[lowerRank - 1] : thresholds[upperRank - 1];
  const Scalar standardError = (z > 0.0) ? 0.5 * (upperBound - lowerBound) / z : 0.0;

  QuantileSimulationResult result(new QuantileSimulationResultImplementation(output_, alpha_, beta_));
  result.setQuantileEstimate(estimate);
  result.setStandardError(standardError);
  result.setConfidenceInterval(Interval(lowerBound, upperBound));
  result.setEstimatorType(estimatorType_);
  result.setCoverageType(QuantileSimulationResultImplementation::COVERAGE_ASYMPTOTIC);
  result.setRequiredCoverage(requiredCoverage_);
  result.setEffectiveSampleSize(1.0 * particleCount);
  result.setSampleSize(sampler.getEvaluationCount());
  result.setIsFixedSample(false);
  result.setTailType(lowerTail ? QuantileSimulationResultImplementation::LOWER_TAIL
                     : QuantileSimulationResultImplementation::UPPER_TAIL);
  result.setThresholdSequence(thresholds);
  result.setStepCount(moveCount);
  result.setAcceptanceRate(sampler.getAcceptanceRate());
  result.setPoissonRanks(lowerRank, upperRank);
  result.setParticleCount(particleCount);
  result.setOuterSampling(moveCount);
  result.setBlockSize(1);
  return result;
}

/* String converter */
String LastParticleQuantile::__repr__() const
{
  OSS oss;
  oss << "class=" << LastParticleQuantile::GetClassName()
      << " output=" << output_
      << " alpha=" << alpha_
      << " beta=" << beta_
      << " particleCount=" << particleCount_
      << " burnIn=" << burnIn_
      << " proposalRange=" << proposalRange_
      << " estimator=" << estimatorType_
      << " requiredCoverage=" << requiredCoverage_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void LastParticleQuantile::save(Advocate & adv) const
{
  QuantileSimulationImplementation::save(adv);
  adv.saveAttribute("particleCount_", particleCount_);
  adv.saveAttribute("burnIn_", burnIn_);
  adv.saveAttribute("proposalRange_", proposalRange_);
}

/* Method load() reloads the object from the StorageManager */
void LastParticleQuantile::load(Advocate & adv)
{
  QuantileSimulationImplementation::load(adv);
  adv.loadAttribute("particleCount_", particleCount_);
  adv.loadAttribute("burnIn_", burnIn_);
  adv.loadAttribute("proposalRange_", proposalRange_);
}

END_NAMESPACE_OPENTURNS

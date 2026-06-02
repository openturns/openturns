//                                               -*- C++ -*-
/**
 *  @brief Factory for truncated distributions
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
#include "openturns/TruncatedDistributionFactory.hxx"
#include "openturns/TruncatedDistribution.hxx"
#include "openturns/MaximumLikelihoodFactory.hxx"
#include "openturns/FittingTest.hxx"
#include "openturns/PersistentObjectFactory.hxx"

#include <limits>

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(TruncatedDistributionFactory)

static const Factory<TruncatedDistributionFactory> Factory_TruncatedDistributionFactory;

/* Default constructor */
TruncatedDistributionFactory::TruncatedDistributionFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Parameters constructor */
TruncatedDistributionFactory::TruncatedDistributionFactory(const Distribution & distribution)
  : DistributionFactoryImplementation()
  , distribution_(distribution)
{
  // Nothing to do
}

/* Virtual constructor */
TruncatedDistributionFactory * TruncatedDistributionFactory::clone() const
{
  return new TruncatedDistributionFactory(*this);
}

/* Here is the interface that all derived class must implement */

TruncatedDistribution TruncatedDistributionFactory::buildAsTruncatedDistribution(const Sample & sample) const
{
  const UnsignedInteger size = sample.getSize();
  const UnsignedInteger dimension = sample.getDimension();

  if (size == 0)
    throw InvalidArgumentException(HERE) << "Error: sample is empty";
  if (dimension != distribution_.getDimension())
    throw InvalidArgumentException(HERE) << "Error: sample/distribution dimension must match";

  const Interval distributionRange(distribution_.getRange());
  const Point dlb(distributionRange.getLowerBound());
  const Point dub(distributionRange.getUpperBound());
  const Interval::BoolCollection dflb(distributionRange.getFiniteLowerBound());
  const Interval::BoolCollection dfub(distributionRange.getFiniteUpperBound());

  // Extend sample bounds slightly beyond the observed data range so that the
  // truncated distribution's support covers all sample points
  const Point slb(sample.getMin() - Point(dimension, 1.0 / size));
  const Point sub(sample.getMax() + Point(dimension, 1.0 / size));

  Collection<Distribution> candidates;
  Indices truncationParamCount;

  // Enumerate all 2^(2d) combinations of truncation
  // Each dimension contributes two bits: bit 2*j for
  // the lower bound and bit 2*j+1 for the upper bound.
  if (2 * dimension >= std::numeric_limits<UnsignedInteger>::digits)
    throw InvalidArgumentException(HERE) << "Error: dimension too large for exhaustive truncation search";
  const UnsignedInteger K = 1UL << (2 * dimension); // 2^(2d)
  for (UnsignedInteger i = 0; i < K; ++i)
  {
    Point clb(dlb);
    Point cub(dub);
    Interval::BoolCollection cflb(dimension, false);
    Interval::BoolCollection cfub(dimension, false);
    UnsignedInteger nBound = 0;
    for (UnsignedInteger j = 0; j < dimension; ++ j)
    {
      // Only try truncating a bound when it would actually tighten the support
      const Bool tryLB = (dflb[j] && dlb[j] < slb[j]) || !dflb[j];
      const Bool tryUB = (dfub[j] && sub[j] < dub[j]) || !dfub[j];
      if (tryLB && (i & (1UL << (2 * j)))) // check if bit 2 * j of i is set
      {
        cflb[j] = 1;
        clb[j] = slb[j];
        ++ nBound;
      }
      if (tryUB && (i & (1UL << (2 * j + 1)))) // check if bit 2 * j + 1 of i is set
      {
        cfub[j] = 1;
        cub[j] = sub[j];
        ++ nBound;
      }
    }

    if (nBound > 0)
    {
      const Interval interval(clb, cub, cflb, cfub);
      candidates.add(TruncatedDistribution(distribution_, interval));
      truncationParamCount.add(nBound);
    }
  }

  if (candidates.getSize() == 0)
    throw InvalidArgumentException(HERE) << "Error: no candidate truncation";

  const UnsignedInteger nNativeParameter = distribution_.getParameterDimension();

  Distribution bestDistribution(distribution_);
  Scalar bestBIC = 0.0;
  if (estimateParameters_)
  {
    // Re-fit the untruncated baseline via MLE for fair BIC comparison
    MaximumLikelihoodFactory baseFactory(distribution_);
    bestDistribution = baseFactory.build(sample);
    bestBIC = FittingTest::BIC(sample, bestDistribution, nNativeParameter);
  }
  else
  {
    bestBIC = FittingTest::BIC(sample, bestDistribution, 0);
  }
  Bool improved = false;

  for (UnsignedInteger i = 0; i < candidates.getSize(); ++i)
  {
    Distribution candidate(candidates[i]);
    const UnsignedInteger nTruncationParam = truncationParamCount[i];
    UnsignedInteger nEstimated = 0;

    if (estimateParameters_)
    {
      MaximumLikelihoodFactory factory(candidates[i]);
      // Set truncation bounds as known parameters
      const Point knownValues(candidate.getParameter());
      Indices knownIndices(nTruncationParam);
      for (UnsignedInteger k = 0; k < nTruncationParam; ++k)
        knownIndices[k] = nNativeParameter + k;
      Point knownParameterValues(nTruncationParam);
      for (UnsignedInteger k = 0; k < nTruncationParam; ++k)
        knownParameterValues[k] = knownValues[nNativeParameter + k];
      factory.setKnownParameter(knownIndices, knownParameterValues);
      candidate = factory.build(sample);
      nEstimated = nNativeParameter;
    }

    // Compare BIC values; lower BIC indicates a better trade-off between
    // goodness-of-fit and model complexity (penalising extra truncation bounds)
    const Scalar bic = FittingTest::BIC(sample, candidate, nEstimated + nTruncationParam);
    if (bic < bestBIC)
    {
      bestBIC = bic;
      bestDistribution = candidate;
      improved = true;
    }
  }

  if (!improved)
    throw InvalidArgumentException(HERE) << "Error: truncation yielded no BIC improvement";

  TruncatedDistribution *p_truncated = dynamic_cast<TruncatedDistribution*>(bestDistribution.getImplementation().get());
  TruncatedDistribution result(*p_truncated);
  result.setDescription(sample.getDescription());
  return result;
}

Distribution TruncatedDistributionFactory::build(const Sample & sample) const
{
  return buildAsTruncatedDistribution(sample);
}

/* Distribution accessor */
void TruncatedDistributionFactory::setDistribution(const Distribution & distribution)
{
  distribution_ = distribution;
}

Distribution TruncatedDistributionFactory::getDistribution() const
{
  return distribution_;
}

/* Estimate parameters flag accessor */
void TruncatedDistributionFactory::setEstimateParameters(const Bool estimateParameters)
{
  estimateParameters_ = estimateParameters;
}

Bool TruncatedDistributionFactory::getEstimateParameters() const
{
  return estimateParameters_;
}

/* Method save() stores the object through the StorageManager */
void TruncatedDistributionFactory::save(Advocate & adv) const
{
  DistributionFactoryImplementation::save(adv);
  adv.saveAttribute("distribution_", distribution_);
  adv.saveAttribute("estimateParameters_", estimateParameters_);
}

/* Method load() reloads the object from the StorageManager */
void TruncatedDistributionFactory::load(Advocate & adv)
{
  DistributionFactoryImplementation::load(adv);
  adv.loadAttribute("distribution_", distribution_);
  adv.loadAttribute("estimateParameters_", estimateParameters_);
}

END_NAMESPACE_OPENTURNS

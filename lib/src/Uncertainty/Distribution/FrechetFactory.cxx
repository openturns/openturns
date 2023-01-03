//                                               -*- C++ -*-
/**
 *  @brief Factory for Frechet distribution
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
#include "openturns/FrechetFactory.hxx"
#include "openturns/GumbelFactory.hxx"
#include "openturns/MaximumLikelihoodFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(FrechetFactory)

static const Factory<FrechetFactory> Factory_FrechetFactory;

/* Default constructor */
FrechetFactory::FrechetFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
FrechetFactory * FrechetFactory::clone() const
{
  return new FrechetFactory(*this);
}

/* Here is the interface that all derived class must implement */

Distribution FrechetFactory::build(const Sample & sample) const
{
  return buildAsFrechet(sample).clone();
}

Distribution FrechetFactory::build(const Point & parameters) const
{
  return buildAsFrechet(parameters).clone();
}

Distribution FrechetFactory::build() const
{
  return buildAsFrechet().clone();
}

Frechet FrechetFactory::buildAsFrechet(const Sample & sample) const
{
  const Scalar size = sample.getSize();
  if (size < 2) throw InvalidArgumentException(HERE) << "Error: cannot build a Frechet distribution from a sample of size < 2";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a Frechet distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();
  const Scalar xMin = sample.getMin()[0];
  const Scalar xMax = sample.getMax()[0];
  Scalar gamma = xMin - std::abs(xMin) / (2.0 + size);
  if (!SpecFunc::IsNormal(gamma)) throw InvalidArgumentException(HERE) << "Error: cannot build a Frechet distribution if data contains NaN or Inf";
  // If the minimum value is zero then one of the shifted values will be zero, leading to an undefined logarithm. The small perturbation is harmless as it is just a matter of getting a reasonable starting point for a further MLE.
  if (xMin == xMax) throw InvalidArgumentException(HERE) << "Error: cannot estimate a LogUniform distribution from a constant sample.";
  // In any case, if the given sample is pathological (lots of zeros, a few positive values) a Frechet distribution is not a plausible candidate.
  if (gamma == 0.0) gamma -= ResourceMap::GetAsScalar("Distribution-DefaultQuantileEpsilon");
  // Convert the translated sample in logarithmic scale, in order for the new sample to be distributed according to the Gumbel distribution
  Sample logSample(size, 1);
  for (UnsignedInteger i = 0; i < size; ++i)
    logSample(i, 0) = std::log(sample(i, 0) - gamma);
  // Estimate the associated Gumbel
  const Gumbel associatedGumbel(GumbelFactory().buildAsGumbel(logSample));
  const Scalar alphaGumbel = 1.0 / associatedGumbel.getBeta();
  const Scalar betaGumbel = associatedGumbel.getGamma();
  // Now get the parameter estimate of the Frechet distribution
  const Scalar alphaFrechet = alphaGumbel;
  const Scalar betaFrechet = std::exp(betaGumbel);
  // Use them to start a MLE estimation
  Frechet model;
  MaximumLikelihoodFactory mleFactory(model);
  OptimizationAlgorithm algo(mleFactory.getOptimizationAlgorithm());
  const Point startingPoint = {betaFrechet, alphaFrechet, gamma};
  algo.setStartingPoint(startingPoint);
  mleFactory.setOptimizationAlgorithm(algo);
  const Scalar margin = std::max(1.0, ResourceMap::GetAsScalar("FrechetFactory-BoundMargin"));
  const Point lower = {betaFrechet / margin, alphaFrechet / margin, gamma - margin * std::abs(gamma)};
  const Point upper = {margin * betaFrechet,  margin * alphaFrechet, gamma + margin * std::abs(gamma)};
  mleFactory.setOptimizationBounds(Interval(lower, upper));
  const Point parameters(mleFactory.buildParameter(sample));
  return buildAsFrechet(parameters);
}

Frechet FrechetFactory::buildAsFrechet(const Point & parameters) const
{
  try
  {
    Frechet distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (const InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a Frechet distribution from the given parameters";
  }
}

Frechet FrechetFactory::buildAsFrechet() const
{
  return Frechet();
}

DistributionFactoryResult FrechetFactory::buildEstimator(const Sample & sample) const
{
  return buildMaximumLikelihoodEstimator(sample, true);
}

END_NAMESPACE_OPENTURNS

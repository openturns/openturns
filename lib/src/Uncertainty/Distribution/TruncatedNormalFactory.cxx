//                                               -*- C++ -*-
/**
 *  @brief Factory for TruncatedNormal distribution
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
#include "openturns/TruncatedNormalFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/MethodOfMomentsFactory.hxx"
#include "openturns/MaximumLikelihoodFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Normal.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(TruncatedNormalFactory)

static const Factory<TruncatedNormalFactory> Factory_TruncatedNormalFactory;

/* Default constructor */
TruncatedNormalFactory::TruncatedNormalFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
TruncatedNormalFactory * TruncatedNormalFactory::clone() const
{
  return new TruncatedNormalFactory(*this);
}


/* Here is the interface that all derived class must implement */

Distribution TruncatedNormalFactory::build(const Sample & sample) const
{
  return buildAsTruncatedNormal(sample).clone();
}

Distribution TruncatedNormalFactory::build(const Point & parameters) const
{
  return buildAsTruncatedNormal(parameters).clone();
}

Distribution TruncatedNormalFactory::build() const
{
  return buildAsTruncatedNormal().clone();
}

/* Algorithm associated with the method of moments */
TruncatedNormal TruncatedNormalFactory::buildMethodOfMoments(const Sample & sample) const
{
  const UnsignedInteger size = sample.getSize();
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a TruncatedNormal distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();
  const Scalar xMin = sample.getMin()[0];
  const Scalar xMax = sample.getMax()[0];
  const Scalar mean = sample.computeMean()[0];
  if (!std::isfinite(mean)) throw InvalidArgumentException(HERE) << "Error: cannot build a TruncatedNormal distribution if data contains NaN or Inf";
  if (xMin == xMax) throw InvalidArgumentException(HERE) << "Error: cannot estimate a TruncatedNormal distribution from a constant sample.";
  Scalar delta = xMax - xMin;
  const Scalar a = xMin - delta / (size + 2);
  const Scalar b = xMax + delta / (size + 2);
  // Create a method of moments
  const Indices momentOrders = {1, 2}; // mean, variance
  MethodOfMomentsFactory factory(build(), momentOrders);

  // Set the bounds as known parameters
  const Point knownParameterValues = {a, b};
  const Indices knownParameterIndices = {2, 3};
  factory.setKnownParameter(knownParameterIndices, knownParameterValues);

  // Compute the mean and std and set it as initial values
  const Scalar sampleMean = sample.computeMean()[0];
  const Scalar sampleSigma = sample.computeStandardDeviation()[0];
  const Point startingPoint = {sampleMean, sampleSigma};

  // Configure starting point
  OptimizationAlgorithm solver(factory.getOptimizationAlgorithm());
  solver.setStartingPoint(startingPoint);
  factory.setOptimizationAlgorithm(solver);

  // Estimate
  const Point parameters(factory.build(sample).getParameter());
  TruncatedNormal result(buildAsTruncatedNormal(parameters));
  result.setDescription(sample.getDescription());
  return result;
}

TruncatedNormal TruncatedNormalFactory::buildMethodOfLikelihoodMaximization(const Sample & sample) const
{
  const UnsignedInteger size = sample.getSize();
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a TruncatedNormal distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();
  // In order to avoid numerical stability issues, we normalize the data to [-1, 1]
  const Scalar xMin = sample.getMin()[0];
  const Scalar xMax = sample.getMax()[0];
  const Scalar mean = sample.computeMean()[0];
  if (!std::isfinite(mean)) throw InvalidArgumentException(HERE) << "Error: cannot build a TruncatedNormal distribution if data contains NaN or Inf";
  if (xMin == xMax) throw InvalidArgumentException(HERE) << "Error: cannot estimate a TruncatedNormal distribution from a constant sample.";

  // X_norm = alpha * (X - beta)
  const Scalar alpha = 2.0 / (xMax - xMin);
  const Scalar beta = 0.5 * (xMin + xMax);
  Sample normalizedSample(sample);
  normalizedSample -= Point(1, beta);
  normalizedSample *= Point(1, alpha);

  const UnsignedInteger dimension = 2;// optimize (mu, sigma)
  Point parametersLowerBound(dimension, SpecFunc::LowestScalar);
  parametersLowerBound[1] = ResourceMap::GetAsScalar("TruncatedNormalFactory-SigmaLowerBound");
  Interval::BoolCollection parametersLowerFlags(dimension, false);
  parametersLowerFlags[1] = true;
  const Point startingPoint = {normalizedSample.computeMean()[0], normalizedSample.computeStandardDeviation()[0]};
  const Scalar oneEps = 1.0 + 1.0 / size;

  MaximumLikelihoodFactory factory(buildAsTruncatedNormal());

  // bounds are fixed
  Point knownParameterValues(2, oneEps);
  knownParameterValues[0] = -oneEps;
  Indices knownParameterIndices(2);
  knownParameterIndices.fill(2);
  factory.setKnownParameter(knownParameterIndices, knownParameterValues);

  // override starting point
  OptimizationAlgorithm solver(factory.getOptimizationAlgorithm());
  solver.setStartingPoint(startingPoint);
  factory.setOptimizationAlgorithm(solver);

  // override bounds
  Interval bounds(parametersLowerBound, Point(dimension, SpecFunc::Infinity), parametersLowerFlags, Interval::BoolCollection(dimension, false));
  factory.setOptimizationBounds(bounds);

  const Point parameters(factory.buildParameter(normalizedSample));

  // The parameters are scaled back
  // X_norm = alpha * (X - beta)
  // X = beta + X_norm / alpha
  const Scalar mu = beta + parameters[0] / alpha;
  const Scalar sigma = parameters[1] / alpha;
  const Scalar a = beta - oneEps / alpha;
  const Scalar b = beta + oneEps / alpha;

  // check if the parameters of the Normal part make sense wrt the bound parameters
  // note that we still want to allow it from the ctor of TruncatedNormal
  // but not in the context of inference as in the chaos we want to avoid such degenerated distributions
  const Scalar epsilon = ResourceMap::GetAsScalar("Distribution-DefaultCDFEpsilon");
  if (Normal(mu, sigma).computeProbability(Interval(a, b)) < epsilon)
    throw InvalidArgumentException(HERE) << "Likelihood-optimized TruncatedNormal is not valid";

  TruncatedNormal result(buildAsTruncatedNormal({mu, sigma, a, b}));

  // abort if distribution is not valid
  if (!std::isfinite(result.getMean()[0]))
    throw InvalidArgumentException(HERE) << "Likelihood-optimized TruncatedNormal is not valid";

  result.setDescription(sample.getDescription());
  adaptToKnownParameter(sample, &result);
  return result;
}

TruncatedNormal TruncatedNormalFactory::buildAsTruncatedNormal(const Sample & sample) const
{
  try
  {
    return buildMethodOfLikelihoodMaximization(sample);
  }
  catch (const Exception &)
  {
    return buildMethodOfMoments(sample);
  }
}

TruncatedNormal TruncatedNormalFactory::buildAsTruncatedNormal(const Point & parameters) const
{
  try
  {
    TruncatedNormal distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (const InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a TruncatedNormal distribution from the given parameters";
  }
}

TruncatedNormal TruncatedNormalFactory::buildAsTruncatedNormal() const
{
  return TruncatedNormal();
}

END_NAMESPACE_OPENTURNS

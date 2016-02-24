//                                               -*- C++ -*-
/**
 *  @brief Factory for Binomial distribution
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/BinomialFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/DiscreteDistribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(BinomialFactory);

/* Default constructor */
BinomialFactory::BinomialFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
BinomialFactory * BinomialFactory::clone() const
{
  return new BinomialFactory(*this);
}


/* Here is the interface that all derived class must implement */

BinomialFactory::Implementation BinomialFactory::build(const NumericalSample & sample) const
{
  return buildAsBinomial(sample).clone();
}

BinomialFactory::Implementation BinomialFactory::build(const NumericalPoint & parameters) const
{
  return buildAsBinomial(parameters).clone();
}

BinomialFactory::Implementation BinomialFactory::build() const
{
  return buildAsBinomial().clone();
}

Binomial BinomialFactory::buildAsBinomial(const NumericalSample & sample) const
{
  const UnsignedInteger size(sample.getSize());
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a Binomial distribution from an empty sample";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a Binomial distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();
  NumericalScalar mean(0.0);
  NumericalScalar var(0.0);
  NumericalScalar sum(0.0);
  UnsignedInteger upperBound(0);
  const NumericalScalar supportEpsilon(ResourceMap::GetAsNumericalScalar("DiscreteDistribution-SupportEpsilon"));
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const NumericalScalar x(sample[i][0]);
    const int iX(static_cast<int>(round(x)));
    // The sample must be made of nonnegative integral values
    if (!(std::abs(x - iX) <= supportEpsilon) || (iX < 0)) throw InvalidArgumentException(HERE) << "Error: can build a Binomial distribution only from a sample made of nonnegative integers, here x=" << x;
    // Update the upper bound
    if (iX > static_cast<const int>(upperBound)) upperBound = iX;
    var = i * var / (i + 1.0) + (1.0 - 1.0 / (i + 1.0)) * (mean - x) * (mean - x) / (i + 1.0);
    mean = (x + i * mean) / (i + 1.0);
    sum += x;
  }
  // Quick exit if sum == 0
  if (sum == 0) return Binomial(1, 0.0);
  // Loop to maximize the likelihood
  // First guess for the parameter n, based on moment estimate
  // mean = np
  // var = np(1-p)
  // p = 1.0 - var / mean
  UnsignedInteger n(upperBound);
  NumericalScalar p(mean / n);
  if (mean > var) n = std::max(upperBound, (UnsignedInteger)round(mean * mean / (mean - var)));
  // Loop over n to get the maximum likelihood
  // First, compute the likelihood resulting from the first estimate
  NumericalScalar logLikelihood(ComputeLogLikelihood(n, p, sample));
  NumericalScalar maxLogLikelihood(logLikelihood);
  UnsignedInteger maxN(n);
  // Check if we have to try the backward direction
  int step(1);
  if (n > upperBound)
  {
    n -= step;
    p = mean / n;
    logLikelihood = ComputeLogLikelihood(n, p, sample);
    // If the first step increased the logLikelihood we go backward
    if (logLikelihood > maxLogLikelihood)
    {
      // Backward
      step = -1;
      maxN = n;
      maxLogLikelihood = logLikelihood;
    }
    // Forward, restore n
    else n += step;
  }
  // Loop over n
  Bool loop(true);
  while (loop)
  {
    n += step;
    p = mean / n;
    logLikelihood = ComputeLogLikelihood(n, p, sample);
    // Loop while the logLikelihood is increasing
    loop = (logLikelihood > maxLogLikelihood);
    if (loop)
    {
      maxN = n;
      maxLogLikelihood = logLikelihood;
    }
    // Check that we don't go too far when in the backward direction
    if (step < 0) loop = loop && (n > upperBound);
  }
  Binomial result(maxN, mean / maxN);
  result.setDescription(sample.getDescription());
  return result;
}

NumericalScalar BinomialFactory::ComputeLogLikelihood(const UnsignedInteger n,
    const NumericalScalar p,
    const NumericalSample & sample)
{
  std::map<UnsignedInteger, NumericalScalar> logLikelihoodCache;
  const UnsignedInteger size(sample.getSize());
  const NumericalScalar logNFactorial(SpecFunc::LnGamma(n + 1.0));
  const NumericalScalar logP(std::log(p));
  const NumericalScalar logQ(log1p(-p));
  NumericalScalar logLikelihood(0.0);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const UnsignedInteger k(static_cast<UnsignedInteger>(round(sample[i][0])));
    if (logLikelihoodCache.find(k) == logLikelihoodCache.end()) logLikelihoodCache[k] = logNFactorial - SpecFunc::LnGamma(n - k + 1.0) - SpecFunc::LnGamma(k + 1.0) + k * logP + (n - k) * logQ;
    logLikelihood += logLikelihoodCache[k];
  }
  return logLikelihood;
}


Binomial BinomialFactory::buildAsBinomial(const NumericalPoint & parameters) const
{
  try
  {
    Binomial distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (InvalidArgumentException)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a Binomial distribution from the given parameters";
  }
}

Binomial BinomialFactory::buildAsBinomial() const
{
  return Binomial();
}

END_NAMESPACE_OPENTURNS

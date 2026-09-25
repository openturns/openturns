//                                               -*- C++ -*-
/**
 *  @brief Factory for MultivariatePolya distribution
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
#include "openturns/MultivariatePolyaFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MultivariatePolyaFactory)

static const Factory<MultivariatePolyaFactory> Factory_MultivariatePolyaFactory;

/* Default constructor */
MultivariatePolyaFactory::MultivariatePolyaFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
MultivariatePolyaFactory * MultivariatePolyaFactory::clone() const
{
  return new MultivariatePolyaFactory(*this);
}

/* Here is the interface that all derived class must implement */

/* Build the distribution from a sample */
Distribution MultivariatePolyaFactory::build(const Sample & sample) const
{
  return buildAsMultivariatePolya(sample).clone();
}

/* Build the distribution from its default parameters */
Distribution MultivariatePolyaFactory::build() const
{
  return buildAsMultivariatePolya().clone();
}

MultivariatePolya MultivariatePolyaFactory::buildAsMultivariatePolya(const Sample & sample) const
{
  if (sample.getSize() < 2) throw InvalidArgumentException(HERE) << "Error: cannot build a MultivariatePolya distribution from a sample of size < 2";
  // Check if each component of the sample is an integer, and that the row sums are constant
  const UnsignedInteger size = sample.getSize();
  const UnsignedInteger dimension = sample.getDimension();
  Point columnSum(dimension, 0.0);
  UnsignedInteger n = 0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    UnsignedInteger sumI = 0;
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      const Scalar x = sample(i, j);
      if (!std::isfinite(x)) throw InvalidArgumentException(HERE) << "Error: cannot build a MultivariatePolya distribution if data contains NaN or Inf";
      if ((x != trunc(x)) || (x < 0.0)) throw InvalidArgumentException(HERE) << "Error: can build a MultivariatePolya distribution only from a sample with positive integer components, here sample[" << i << "][" << j << "]=" << x;
      sumI += static_cast<UnsignedInteger>(x);
      columnSum[j] += x;
    }
    if (i == 0) n = sumI;
    else if (sumI != n) throw InvalidArgumentException(HERE) << "Error: the number of draws must be constant along the sample, here row " << i << " sums to " << sumI << " while the first row sums to " << n;
  }
  if (n == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a MultivariatePolya distribution from a sample with all zero entries";
  // Estimate the proportions from the column means and the overdispersion from the
  // ratio of the sample variance to the binomial variance, see the report sec:factory
  Point proportions(dimension);
  Point columnVariance(dimension, 0.0);
  for (UnsignedInteger j = 0; j < dimension; ++j)
  {
    const Scalar meanJ = columnSum[j] / size;
    if (meanJ == 0.0) throw InvalidArgumentException(HERE) << "Error: cannot build a MultivariatePolya distribution if an observed category has a zero count, as the concentration parameters must be strictly positive";
    proportions[j] = meanJ / n;
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      const Scalar diff = sample(i, j) - meanJ;
      columnVariance[j] += diff * diff;
    }
    columnVariance[j] /= (size - 1.0);
  }
  // Overdispersion factor of each component, clamped to the feasible range [1, n].
  // The underlying counts are mixed binomials, so the standard deviation of each
  // dispersion estimate is about sqrt(2 / (size - 1)); a genuine multinomial sample
  // is not declared overdispersed unless the average ratio is significantly above 1.
  const Scalar dispersionThreshold = 1.0 + 3.0 * std::sqrt(2.0 / (size - 1.0));
  Scalar overdispersionMean = 0.0;
  UnsignedInteger count = 0;
  for (UnsignedInteger j = 0; j < dimension; ++j)
  {
    const Scalar binomialVariance = n * proportions[j] * (1.0 - proportions[j]);
    if (binomialVariance <= SpecFunc::Precision) continue;
    const Scalar ratio = std::min(n * 1.0, std::max(1.0, columnVariance[j] / binomialVariance));
    overdispersionMean += ratio;
    ++count;
  }
  Scalar c = 1.0;
  Point alpha(dimension);
  if ((count > 0) && (overdispersionMean / count > dispersionThreshold))
  {
    const Scalar overdispersion = overdispersionMean / count;
    const Scalar theta = (n - overdispersion) / (overdispersion - 1.0);
    if (theta > 0.0)
    {
      for (UnsignedInteger j = 0; j < dimension; ++j) alpha[j] = theta * proportions[j];
    }
    else
    {
      // The overdispersion is out of the range where the multivariate Polya
      // model with the observed moments exists, so the sample is not overdispersed
      for (UnsignedInteger j = 0; j < dimension; ++j) alpha[j] = proportions[j];
      c = 0.0;
    }
  }
  else
  {
    // The sample is not significantly overdispersed, the degenerate multinomial member is returned
    for (UnsignedInteger j = 0; j < dimension; ++j) alpha[j] = proportions[j];
    c = 0.0;
  }
  MultivariatePolya result(n, alpha, c);
  result.setDescription(sample.getDescription());
  adaptToKnownParameter(sample, &result);
  return result;
}

/* Build the distribution from its default parameters */
MultivariatePolya MultivariatePolyaFactory::buildAsMultivariatePolya() const
{
  return MultivariatePolya();
}

/* Method save() stores the object through the StorageManager */
void MultivariatePolyaFactory::save(Advocate & adv) const
{
  DistributionFactoryImplementation::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void MultivariatePolyaFactory::load(Advocate & adv)
{
  DistributionFactoryImplementation::load(adv);
}

END_NAMESPACE_OPENTURNS
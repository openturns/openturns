//                                               -*- C++ -*-
/**
 *  @brief Factory for MultivariateHypergeometric distribution
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
#include "openturns/MultivariateHypergeometricFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MultivariateHypergeometricFactory)

static const Factory<MultivariateHypergeometricFactory> Factory_MultivariateHypergeometricFactory;

/* Default constructor */
MultivariateHypergeometricFactory::MultivariateHypergeometricFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
MultivariateHypergeometricFactory * MultivariateHypergeometricFactory::clone() const
{
  return new MultivariateHypergeometricFactory(*this);
}

/* Here is the interface that all derived class must implement */

/* Build the distribution from a sample */
Distribution MultivariateHypergeometricFactory::build(const Sample & sample) const
{
  return buildAsMultivariateHypergeometric(sample).clone();
}

/* Build the distribution from its default parameters */
Distribution MultivariateHypergeometricFactory::build() const
{
  return buildAsMultivariateHypergeometric().clone();
}

/* Build the distribution from a sample */
MultivariateHypergeometric MultivariateHypergeometricFactory::buildAsMultivariateHypergeometric(const Sample & sample) const
{
  if (sample.getSize() < 2) throw InvalidArgumentException(HERE) << "Error: cannot build a MultivariateHypergeometric distribution from a sample of size < 2";
  // Check if each component of the sample is an integer, and that the row sums are constant
  UnsignedInteger size = sample.getSize();
  UnsignedInteger dimension = sample.getDimension();
  Point colorsSum(dimension, 0.0);
  UnsignedInteger n = 0;
  for (UnsignedInteger i = 0; i < size; i++)
  {
    UnsignedInteger sumI = 0;
    for (UnsignedInteger j = 0; j < dimension; j++)
    {
      Scalar x = sample(i, j);
      if (!std::isfinite(x)) throw InvalidArgumentException(HERE) << "Error: cannot build a MultivariateHypergeometric distribution if data contains NaN or Inf";
      if ((x != trunc(x)) || (x < 0.0)) throw InvalidArgumentException(HERE) << "Error: can build a MultivariateHypergeometric distribution only from a sample with positive integer components, here sample[" << i << "][" << j << "]=" << x;
      sumI += static_cast<UnsignedInteger>(x);
      colorsSum[j] += x;
    }
    if (i == 0) n = sumI;
    else if (sumI != n) throw InvalidArgumentException(HERE) << "Error: the number of draws must be constant along the sample, here row " << i << " sums to " << sumI << " while the first row sums to " << n;
  }
  if (n == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a MultivariateHypergeometric distribution from a sample with all zero entries";
  // Estimate the proportions, then choose the smallest population size making all the
  // proportions rounded counts positive (for the observed categories),
  // at least as large as the largest count observed in each category,
  // and the draw feasible
  Point proportions(dimension);
  for (UnsignedInteger j = 0; j < dimension; ++j)
    proportions[j] = colorsSum[j] / (n * size);
  Scalar minPositiveProportion = 1.0;
  for (UnsignedInteger j = 0; j < dimension; ++j)
  {
    if (proportions[j] > 0.0) minPositiveProportion = std::min(minPositiveProportion, proportions[j]);
  }
  UnsignedInteger K = static_cast< UnsignedInteger >(std::max(1.0 * n, std::ceil(1.0 / minPositiveProportion)));
  // The sample components are bounded wrt their sum, so the estimate cannot
  // give a category a population count smaller than the largest one observed
  const Point maxObserved(sample.getMax());
  for (UnsignedInteger j = 0; j < dimension; ++j)
  {
    if (proportions[j] > 0.0) K = std::max(K, static_cast< UnsignedInteger >(std::ceil(maxObserved[j] / proportions[j])));
  }
  Point colors(dimension);
  Bool feasible = false;
  while (!feasible)
  {
    // Safety guard: the total population cannot be less than the number of draws
    if (K < n) K = n;
    UnsignedInteger sumColors = 0;
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      colors[j] = (proportions[j] == 0.0 ? 0.0 : std::max(1.0, std::round(K * proportions[j])));
      sumColors += static_cast<UnsignedInteger>(colors[j]);
    }
    feasible = (sumColors >= n);
    for (UnsignedInteger j = 0; j < dimension; ++j)
      feasible = feasible && (colors[j] >= maxObserved[j]);
    ++K;
  }
  MultivariateHypergeometric result(n, colors);
  result.setDescription(sample.getDescription());
  adaptToKnownParameter(sample, &result);
  return result;
}

/* Build the distribution from its default parameters */
MultivariateHypergeometric MultivariateHypergeometricFactory::buildAsMultivariateHypergeometric() const
{
  return MultivariateHypergeometric();
}

/* Method save() stores the object through the StorageManager */
void MultivariateHypergeometricFactory::save(Advocate & adv) const
{
  DistributionFactoryImplementation::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void MultivariateHypergeometricFactory::load(Advocate & adv)
{
  DistributionFactoryImplementation::load(adv);
}

END_NAMESPACE_OPENTURNS
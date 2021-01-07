//                                               -*- C++ -*-
/**
 *  @brief Factory for Multinomial distribution
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/MultinomialFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MultinomialFactory)

static const Factory<MultinomialFactory> Factory_MultinomialFactory;

/* Default constructor */
MultinomialFactory::MultinomialFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
MultinomialFactory * MultinomialFactory::clone() const
{
  return new MultinomialFactory(*this);
}


/* Here is the interface that all derived class must implement */

Distribution MultinomialFactory::build(const Sample & sample) const
{
  return buildAsMultinomial(sample).clone();
}

Distribution MultinomialFactory::build() const
{
  return buildAsMultinomial().clone();
}

Multinomial MultinomialFactory::buildAsMultinomial(const Sample & sample) const
{
  if (sample.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a Multinomial distribution from an empty sample";
  // Check if each component of the sample is an integer, compute the mean and extract the maximum value
  UnsignedInteger size = sample.getSize();
  UnsignedInteger dimension = sample.getDimension();
  Point p(dimension, 0.0);
  UnsignedInteger maxSum = 0;
  for (UnsignedInteger i = 0; i < size; i++)
  {
    UnsignedInteger sumI = 0;
    for (UnsignedInteger j = 0; j < dimension; j++)
    {
      Scalar x = sample(i, j);
      if ((x != trunc(x)) || (x < 0.0)) throw InvalidArgumentException(HERE) << "Error: can build a Multinomial distribution only from a sample with positive integer components, here sample[" << i << "][" << j << "]=" << x;
      sumI += static_cast<UnsignedInteger>(x);
      p[j] += x;
    }
    if (sumI > maxSum)
      maxSum = sumI;
  }
  p *= 1.0 / (maxSum * size);
  Multinomial result(maxSum, p);
  result.setDescription(sample.getDescription());
  return result;
}

Multinomial MultinomialFactory::buildAsMultinomial() const
{
  return Multinomial();
}

END_NAMESPACE_OPENTURNS

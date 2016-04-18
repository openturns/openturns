//                                               -*- C++ -*-
/**
 *  @brief Factory for Multinomial distribution
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
#include <cmath>
#include "openturns/MultinomialFactory.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MultinomialFactory);

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

MultinomialFactory::Implementation MultinomialFactory::build(const NumericalSample & sample) const
{
  return buildAsMultinomial(sample).clone();
}

MultinomialFactory::Implementation MultinomialFactory::build() const
{
  return buildAsMultinomial().clone();
}

Multinomial MultinomialFactory::buildAsMultinomial(const NumericalSample & sample) const
{
  if (sample.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a Multinomial distribution from an empty sample";
  // Check if each component of the sample is an integer, compute the mean and extract the maximum value
  UnsignedInteger size = sample.getSize();
  UnsignedInteger dimension = sample.getDimension();
  NumericalPoint p(dimension, 0.0);
  NumericalScalar max = sample[0][0];
  for (UnsignedInteger i = 0; i < size; i++)
  {
    for (UnsignedInteger j = 0; j < dimension; j++)
    {
      NumericalScalar x = sample[i][j];
      if ((x != trunc(x)) || (x < 0.0)) throw InvalidArgumentException(HERE) << "Error: can build a Multinomial distribution only from a sample with positive integer components, here sample[" << i << "][" << j << "]=" << x;
      if (x > max) max = x;
      p[j] += x;
    }
  }
  const UnsignedInteger n = (UnsignedInteger)max;
  p *= 1.0 / (max * size);
  Multinomial result(n, p);
  result.setDescription(sample.getDescription());
  return result;
}

Multinomial MultinomialFactory::buildAsMultinomial() const
{
  return Multinomial();
}

END_NAMESPACE_OPENTURNS

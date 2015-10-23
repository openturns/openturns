//                                               -*- C++ -*-
/**
 *  @brief Factory for Geometric distribution
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#include "GeometricFactory.hxx"
#include "SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GeometricFactory);

/* Default constructor */
GeometricFactory::GeometricFactory()
  : DistributionImplementationFactory()
{
  // Nothing to do
}

/* Virtual constructor */
GeometricFactory * GeometricFactory::clone() const
{
  return new GeometricFactory(*this);
}


/* Here is the interface that all derived class must implement */

GeometricFactory::Implementation GeometricFactory::build(const NumericalSample & sample) const
{
  return buildAsGeometric(sample).clone();
}

GeometricFactory::Implementation GeometricFactory::build(const NumericalPoint & parameters) const
{
  return buildAsGeometric(parameters).clone();
}

GeometricFactory::Implementation GeometricFactory::build() const
{
  return buildAsGeometric().clone();
}

Geometric GeometricFactory::buildAsGeometric(const NumericalSample & sample) const
{
  if (sample.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a Geometric distribution from an empty sample";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a Geometric distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();
  // Check if all the components of the sample are integers > 0
  UnsignedInteger size(sample.getSize());
  NumericalScalar sum(0.0);
  for (UnsignedInteger i = 0; i < size; i++)
  {
    NumericalScalar x(sample[i][0]);
    if ((x != trunc(x)) || (x < 1.0)) throw InvalidArgumentException(HERE) << "Error: can build a Geometric distribution only from a sample with integer components >= 1, here sample[" << i << "][0]=" << x;
    sum += x;
  }
  Geometric result(size / sum);
  result.setDescription(sample.getDescription());
  return result;
}

Geometric GeometricFactory::buildAsGeometric(const NumericalPoint & parameters) const
{
  try
  {
    Geometric distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (InvalidArgumentException)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a Geometric distribution from the given parameters";
  }
}

Geometric GeometricFactory::buildAsGeometric() const
{
  return Geometric();
}

END_NAMESPACE_OPENTURNS

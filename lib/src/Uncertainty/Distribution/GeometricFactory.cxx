//                                               -*- C++ -*-
/**
 *  @brief Factory for Geometric distribution
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
#include "openturns/GeometricFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GeometricFactory)

static const Factory<GeometricFactory> Factory_GeometricFactory;

/* Default constructor */
GeometricFactory::GeometricFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
GeometricFactory * GeometricFactory::clone() const
{
  return new GeometricFactory(*this);
}


/* Here is the interface that all derived class must implement */

Distribution GeometricFactory::build(const Sample & sample) const
{
  return buildAsGeometric(sample).clone();
}

Distribution GeometricFactory::build(const Point & parameters) const
{
  return buildAsGeometric(parameters).clone();
}

Distribution GeometricFactory::build() const
{
  return buildAsGeometric().clone();
}

Geometric GeometricFactory::buildAsGeometric(const Sample & sample) const
{
  if (sample.getSize() < 2) throw InvalidArgumentException(HERE) << "Error: cannot build a Geometric distribution from a sample of size < 2";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a Geometric distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();
  // Check if all the components of the sample are integers > 0
  UnsignedInteger size = sample.getSize();
  Scalar sum = 0.0;
  for (UnsignedInteger i = 0; i < size; i++)
  {
    Scalar x = sample(i, 0);
    if ((x != trunc(x)) || (x < 1.0)) throw InvalidArgumentException(HERE) << "Error: can build a Geometric distribution only from a sample with integer components >= 1, here sample[" << i << "][0]=" << x;
    sum += x;
  }
  Geometric result(size / sum);
  result.setDescription(sample.getDescription());
  return result;
}

Geometric GeometricFactory::buildAsGeometric(const Point & parameters) const
{
  try
  {
    Geometric distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a Geometric distribution from the given parameters";
  }
}

Geometric GeometricFactory::buildAsGeometric() const
{
  return Geometric();
}

END_NAMESPACE_OPENTURNS

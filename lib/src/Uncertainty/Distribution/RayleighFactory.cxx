//                                               -*- C++ -*-
/**
 *  @brief Factory for Rayleigh distribution
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
#include "RayleighFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(RayleighFactory);

/* Default constructor */
RayleighFactory::RayleighFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
RayleighFactory * RayleighFactory::clone() const
{
  return new RayleighFactory(*this);
}

/* Here is the interface that all derived class must implement */

RayleighFactory::Implementation RayleighFactory::build(const NumericalSample & sample) const
{
  return buildAsRayleigh(sample).clone();
}

RayleighFactory::Implementation RayleighFactory::build(const NumericalPoint & parameters) const
{
  return buildAsRayleigh(parameters).clone();
}

RayleighFactory::Implementation RayleighFactory::build() const
{
  return buildAsRayleigh().clone();
}

Rayleigh RayleighFactory::buildAsRayleigh(const NumericalSample & sample) const
{
  const UnsignedInteger size(sample.getSize());
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a Rayleigh distribution from an empty sample";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a Rayleigh distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();
  const NumericalScalar xMin(sample.getMin()[0]);
  const NumericalScalar gamma(xMin - std::abs(xMin) / (2.0 + size));
  NumericalScalar sumSquares(0.0);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const NumericalScalar xI(sample[i][0] - gamma);
    sumSquares += xI * xI;
  }
  try
  {
    Rayleigh result(std::sqrt(0.5 * sumSquares / size), gamma);
    result.setDescription(sample.getDescription());
    return result;
  }
  catch (InvalidArgumentException)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot estimate parameters of a Rayleigh distribution from the given sample";
  }
}

Rayleigh RayleighFactory::buildAsRayleigh(const NumericalPoint & parameters) const
{
  try
  {
    Rayleigh distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (InvalidArgumentException)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a Rayleigh distribution from the given parameters";
  }
}

Rayleigh RayleighFactory::buildAsRayleigh() const
{
  return Rayleigh();
}

END_NAMESPACE_OPENTURNS

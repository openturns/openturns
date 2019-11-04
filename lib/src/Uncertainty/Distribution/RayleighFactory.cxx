//                                               -*- C++ -*-
/**
 *  @brief Factory for Rayleigh distribution
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/RayleighFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(RayleighFactory)

static const Factory<RayleighFactory> Factory_RayleighFactory;

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

Distribution RayleighFactory::build(const Sample & sample) const
{
  return buildAsRayleigh(sample).clone();
}

Distribution RayleighFactory::build(const Point & parameters) const
{
  return buildAsRayleigh(parameters).clone();
}

Distribution RayleighFactory::build() const
{
  return buildAsRayleigh().clone();
}

Rayleigh RayleighFactory::buildAsRayleigh(const Sample & sample) const
{
  const UnsignedInteger size = sample.getSize();
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a Rayleigh distribution from an empty sample";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a Rayleigh distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();
  const Scalar xMin = sample.getMin()[0];
  const Scalar gamma = xMin - std::abs(xMin) / (2.0 + size);
  Scalar sumSquares = 0.0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Scalar xI = sample(i, 0) - gamma;
    sumSquares += xI * xI;
  }
  // Here we test on sumSquares in order to detect also overflows
  if (!SpecFunc::IsNormal(sumSquares)) throw InvalidArgumentException(HERE) << "Error: cannot build a Rayleigh distribution if data contains NaN or Inf";
  if (sumSquares == 0.0)
  {
    Rayleigh result(100.0 * (std::max(std::abs(gamma), SpecFunc::ScalarEpsilon) * SpecFunc::ScalarEpsilon), gamma);
    result.setDescription(sample.getDescription());
    return result;
  }
  try
  {
    Rayleigh result(std::sqrt(0.5 * sumSquares / size), gamma);
    result.setDescription(sample.getDescription());
    return result;
  }
  catch (InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot estimate parameters of a Rayleigh distribution from the given sample";
  }
}

Rayleigh RayleighFactory::buildAsRayleigh(const Point & parameters) const
{
  try
  {
    Rayleigh distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a Rayleigh distribution from the given parameters";
  }
}

Rayleigh RayleighFactory::buildAsRayleigh() const
{
  return Rayleigh();
}

END_NAMESPACE_OPENTURNS

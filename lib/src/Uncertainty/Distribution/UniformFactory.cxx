//                                               -*- C++ -*-
/**
 *  @brief Factory for Uniform distribution
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/UniformFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(UniformFactory)

static const Factory<UniformFactory> Factory_UniformFactory;

/* Default constructor */
UniformFactory::UniformFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
UniformFactory * UniformFactory::clone() const
{
  return new UniformFactory(*this);
}


/* Here is the interface that all derived class must implement */

Distribution UniformFactory::build(const Sample & sample) const
{
  return buildAsUniform(sample).clone();
}

Distribution UniformFactory::build(const Point & parameters) const
{
  return buildAsUniform(parameters).clone();
}

Distribution UniformFactory::build() const
{
  return buildAsUniform().clone();
}

Uniform UniformFactory::buildAsUniform(const Sample & sample) const
{
  const Scalar size = sample.getSize();
  if (size < 2) throw InvalidArgumentException(HERE) << "Error: cannot build a Uniform distribution from a sample of size < 2";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a Uniform distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();
  const Scalar xMin = sample.getMin()[0];
  const Scalar xMax = sample.getMax()[0];
  Scalar delta = xMax - xMin;
  const Scalar a = xMin - delta / (size + 2);
  const Scalar b = xMax + delta / (size + 2);
  if (!SpecFunc::IsNormal(a) || !SpecFunc::IsNormal(b)) throw InvalidArgumentException(HERE) << "Error: cannot build a Uniform distribution if data contains NaN or Inf";
  if (xMin == xMax)
  {
    delta = std::max(std::abs(xMin), 10.0) * SpecFunc::ScalarEpsilon;
    Uniform result(xMin - delta, xMax + delta);
    result.setDescription(sample.getDescription());
    return result;
  }
  Uniform result(a, b);
  result.setDescription(sample.getDescription());
  return result;
}

Uniform UniformFactory::buildAsUniform(const Point & parameters) const
{
  try
  {
    Uniform distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a Uniform distribution from the given parameters";
  }
}

Uniform UniformFactory::buildAsUniform() const
{
  return Uniform();
}

END_NAMESPACE_OPENTURNS

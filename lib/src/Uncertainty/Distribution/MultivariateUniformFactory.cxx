//                                               -*- C++ -*-
/**
 *  @brief Factory for MultivariateUniform distribution
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
#include "openturns/MultivariateUniformFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MultivariateUniformFactory)

static const Factory<MultivariateUniformFactory> Factory_MultivariateUniformFactory;

/* Default constructor */
MultivariateUniformFactory::MultivariateUniformFactory():
  DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
MultivariateUniformFactory * MultivariateUniformFactory::clone() const
{
  return new MultivariateUniformFactory(*this);
}


/* Here is the interface that all derived class must implement */

Distribution MultivariateUniformFactory::build(const Sample & sample) const
{
  return buildAsMultivariateUniform(sample).clone();
}

Distribution MultivariateUniformFactory::build(const Point & parameters) const
{
  return buildAsMultivariateUniform(parameters).clone();
}

Distribution MultivariateUniformFactory::build() const
{
  return buildAsMultivariateUniform().clone();
}

MultivariateUniform MultivariateUniformFactory::buildAsMultivariateUniform(const Sample & sample) const
{
  const Scalar size = sample.getSize();
  if (size < 2) throw InvalidArgumentException(HERE) << "Error: cannot build a MultivariateUniform distribution from a sample of size < 2";
  const UnsignedInteger dimension = sample.getDimension();
  const Point xMin(sample.getMin());
  const Point xMax(sample.getMax());
  const Scalar invFactor = 1.0 / (size + 2.0);
  Point a(dimension);
  Point b(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    if (!(xMin[i] < xMax[i]))
      throw InvalidArgumentException(HERE) << "MultivariateUniform cannot estimate a distribution from a constant/nan/inf sample for component " << i;
    const Scalar delta = xMax[i] - xMin[i];
    a[i] = xMin[i] - delta * invFactor;
    b[i] = xMax[i] + delta * invFactor;
  }
  MultivariateUniform result(a, b);
  result.setDescription(sample.getDescription());
  adaptToKnownParameter(sample, &result);
  return result;
}

MultivariateUniform MultivariateUniformFactory::buildAsMultivariateUniform(const Point & parameters) const
{
  try
  {
    MultivariateUniform distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (const InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a MultivariateUniform distribution from the given parameters";
  }
}

MultivariateUniform MultivariateUniformFactory::buildAsMultivariateUniform() const
{
  return MultivariateUniform();
}

END_NAMESPACE_OPENTURNS

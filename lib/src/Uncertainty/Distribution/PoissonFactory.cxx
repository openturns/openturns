//                                               -*- C++ -*-
/**
 *  @brief Factory for Poisson distribution
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
#include "openturns/PoissonFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(PoissonFactory)

static const Factory<PoissonFactory> Factory_PoissonFactory;

/* Default constructor */
PoissonFactory::PoissonFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
PoissonFactory * PoissonFactory::clone() const
{
  return new PoissonFactory(*this);
}

/* Here is the interface that all derived class must implement */

Distribution PoissonFactory::build(const Sample & sample) const
{
  return buildAsPoisson(sample).clone();
}

Distribution PoissonFactory::build(const Point & parameters) const
{
  return buildAsPoisson(parameters).clone();
}

Distribution PoissonFactory::build() const
{
  return buildAsPoisson().clone();
}

Poisson PoissonFactory::buildAsPoisson(const Sample & sample) const
{
  if (sample.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a Poisson distribution from an empty sample";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a Poisson distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();
  // Check if all the components of the sample are integers > 0
  const UnsignedInteger size = sample.getSize();
  Scalar lambda = 0.0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    Scalar x = sample(i, 0);
    if ((x != trunc(x)) || (x < 0.0)) throw InvalidArgumentException(HERE) << "Error: can build a Poisson distribution only from a sample with integer components >= 0, here sample[" << i << "][0]=" << x;
    lambda += x;
  }
  if (!(lambda > 0.0) || SpecFunc::IsInf(lambda)) throw InvalidArgumentException(HERE) << "Error: can build a poisson distribution only if lambda > 0, here lambda=" << lambda;
  Poisson result(lambda / size);
  result.setDescription(sample.getDescription());
  return result;
}

Poisson PoissonFactory::buildAsPoisson(const Point & parameters) const
{
  try
  {
    Poisson distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a Poisson distribution from the given parameters";
  }
}

Poisson PoissonFactory::buildAsPoisson() const
{
  return Poisson();
}

END_NAMESPACE_OPENTURNS

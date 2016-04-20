//                                               -*- C++ -*-
/**
 *  @brief Factory for Laplace distribution
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
#include "openturns/LaplaceFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(LaplaceFactory);

/* Default constructor */
LaplaceFactory::LaplaceFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
LaplaceFactory * LaplaceFactory::clone() const
{
  return new LaplaceFactory(*this);
}

/* Here is the interface that all derived class must implement */

LaplaceFactory::Implementation LaplaceFactory::build(const NumericalSample & sample) const
{
  return buildAsLaplace(sample).clone();
}

LaplaceFactory::Implementation LaplaceFactory::build(const NumericalPoint & parameters) const
{
  return buildAsLaplace(parameters).clone();
}

LaplaceFactory::Implementation LaplaceFactory::build() const
{
  return buildAsLaplace().clone();
}

Laplace LaplaceFactory::buildAsLaplace(const NumericalSample & sample) const
{
  if (sample.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a Laplace distribution from an empty sample";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a Laplace distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();
  const UnsignedInteger size = sample.getSize();
  const NumericalScalar mu = sample.computeMedian()[0];
  NumericalScalar tau = 0.0;
  for (UnsignedInteger i = 0; i < size; ++i) tau += std::abs(sample[i][0] - mu);
  if (tau == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a Laplace distribution with infinite lambda.";
  Laplace result(size / tau, mu);
  result.setDescription(sample.getDescription());
  return result;
}

Laplace LaplaceFactory::buildAsLaplace(const NumericalPoint & parameters) const
{
  try
  {
    Laplace distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (InvalidArgumentException)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a Laplace distribution from the given parameters";
  }
}

Laplace LaplaceFactory::buildAsLaplace() const
{
  return Laplace();
}

END_NAMESPACE_OPENTURNS

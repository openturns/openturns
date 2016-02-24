//                                               -*- C++ -*-
/**
 *  @brief Factory for Skellam distribution
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
#include "openturns/SkellamFactory.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SkellamFactory);

/* Default constructor */
SkellamFactory::SkellamFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
SkellamFactory * SkellamFactory::clone() const
{
  return new SkellamFactory(*this);
}

/* Here is the interface that all derived class must implement */

SkellamFactory::Implementation SkellamFactory::build(const NumericalSample & sample) const
{
  return buildAsSkellam(sample).clone();
}

SkellamFactory::Implementation SkellamFactory::build(const NumericalPoint & parameters) const
{
  return buildAsSkellam(parameters).clone();
}

SkellamFactory::Implementation SkellamFactory::build() const
{
  return buildAsSkellam().clone();
}

DistributionFactoryResult SkellamFactory::buildEstimator(const NumericalSample & sample) const
{
  return buildBootStrapEstimator(sample, true);
}

Skellam SkellamFactory::buildAsSkellam(const NumericalSample & sample) const
{
  if (sample.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a Skellam distribution from an empty sample";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a Skellam distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();
  // Check if all the components of the sample are integers
  const UnsignedInteger size(sample.getSize());
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    NumericalScalar x(sample[i][0]);
    if (x != trunc(x)) throw InvalidArgumentException(HERE) << "Error: can build a Skellam distribution only from a sample with integer components, here sample[" << i << "][0]=" << x;
  }
  const NumericalScalar mean(sample.computeMean()[0]);
  const NumericalScalar var(sample.computeVariance()[0]);
  const NumericalScalar lambda2(0.5 * (var - mean));
  if (lambda2 <= 0.0) throw InvalidArgumentException(HERE) << "Error: can build a skellam distribution only if lambda2 > 0, here lambda2=" << lambda2;
  const NumericalScalar lambda1(0.5 * (var + mean));
  Skellam result(lambda1, lambda2);
  result.setDescription(sample.getDescription());
  return result;
}

Skellam SkellamFactory::buildAsSkellam(const NumericalPoint & parameters) const
{
  try
  {
    Skellam distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (InvalidArgumentException)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a Skellam distribution from the given parameters";
  }
}

Skellam SkellamFactory::buildAsSkellam() const
{
  return Skellam();
}

END_NAMESPACE_OPENTURNS

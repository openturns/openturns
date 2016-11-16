//                                               -*- C++ -*-
/**
 *  @brief Factory for Dirac distribution
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
#include "openturns/DiracFactory.hxx"
#include "openturns/DiscreteDistribution.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(DiracFactory);

static const Factory<DiracFactory> Factory_DiracFactory;

/* Default constructor */
DiracFactory::DiracFactory():
  DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
DiracFactory * DiracFactory::clone() const
{
  return new DiracFactory(*this);
}


/* Here is the interface that all derived class must implement */

DiracFactory::Implementation DiracFactory::build(const NumericalSample & sample) const
{
  return buildAsDirac(sample).clone();
}

DiracFactory::Implementation DiracFactory::build(const NumericalPoint & parameters) const
{
  return buildAsDirac(parameters).clone();
}

DiracFactory::Implementation DiracFactory::build() const
{
  return buildAsDirac().clone();
}

Dirac DiracFactory::buildAsDirac(const NumericalSample & sample) const
{
  const UnsignedInteger size = sample.getSize();
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a Dirac distribution from an empty sample";
  NumericalPoint point(sample[0]);
  // Check that all the points are equal up to the discrete distribution support precision
  const NumericalScalar supportEpsilon = ResourceMap::GetAsNumericalScalar("DiscreteDistribution-SupportEpsilon");
  for (UnsignedInteger i = 1; i < size; ++i) if ((point - sample[i]).norm() > supportEpsilon) throw InvalidArgumentException(HERE) << "Error: cannot build a Dirac distribution from a sample containing different points";
  Dirac result(point);
  result.setDescription(sample.getDescription());
  return result;
}

Dirac DiracFactory::buildAsDirac(const NumericalPoint & parameters) const
{
  try
  {
    Dirac distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (InvalidArgumentException)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a Dirac distribution from the given parameters";
  }
}

Dirac DiracFactory::buildAsDirac() const
{
  return Dirac();
}

END_NAMESPACE_OPENTURNS

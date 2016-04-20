//                                               -*- C++ -*-
/**
 *  @brief Factory for GumbelCopula distribution
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
#include "openturns/GumbelCopulaFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GumbelCopulaFactory);

/* Default constructor */
GumbelCopulaFactory::GumbelCopulaFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
GumbelCopulaFactory * GumbelCopulaFactory::clone() const
{
  return new GumbelCopulaFactory(*this);
}

/* Here is the interface that all derived class must implement */

GumbelCopulaFactory::Implementation GumbelCopulaFactory::build(const NumericalSample & sample) const
{
  return buildAsGumbelCopula(sample).clone();
}

GumbelCopulaFactory::Implementation GumbelCopulaFactory::build(const NumericalPoint & parameters) const
{
  return buildAsGumbelCopula(parameters).clone();
}

GumbelCopulaFactory::Implementation GumbelCopulaFactory::build() const
{
  return buildAsGumbelCopula().clone();
}

GumbelCopula GumbelCopulaFactory::buildAsGumbelCopula(const NumericalSample & sample) const
{
  if (sample.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a GumbelCopula distribution from an empty sample";
  if (sample.getDimension() != 2) throw InvalidArgumentException(HERE) << "Error: cannot build a GumbelCopula distribution from a sample of dimension not equal to 2";
  NumericalScalar tau = sample.computeKendallTau().operator()(0, 1);
  if (tau == 1) throw InvalidArgumentException(HERE) << "Error: cannot build a GumbelCopula distribution from a sample with Kendall tau equal to 1";
  GumbelCopula result(1.0 / (1.0 - tau));
  result.setDescription(sample.getDescription());
  return result;
}

GumbelCopula GumbelCopulaFactory::buildAsGumbelCopula(const NumericalPoint & parameters) const
{
  try
  {
    GumbelCopula copula;
    copula.setParameter(parameters);
    return copula;
  }
  catch (InvalidArgumentException)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a GumbelCopula from the given parameters";
  }
}

GumbelCopula GumbelCopulaFactory::buildAsGumbelCopula() const
{
  return GumbelCopula();
}


END_NAMESPACE_OPENTURNS

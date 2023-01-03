//                                               -*- C++ -*-
/**
 *  @brief Factory for ClaytonCopula distribution
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/ClaytonCopulaFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ClaytonCopulaFactory)

static const Factory<ClaytonCopulaFactory> Factory_ClaytonCopulaFactory;

/* Default constructor */
ClaytonCopulaFactory::ClaytonCopulaFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
ClaytonCopulaFactory * ClaytonCopulaFactory::clone() const
{
  return new ClaytonCopulaFactory(*this);
}

/* Here is the interface that all derived class must implement */

Distribution ClaytonCopulaFactory::build(const Sample & sample) const
{
  return buildAsClaytonCopula(sample).clone();
}

Distribution ClaytonCopulaFactory::build(const Point & parameters) const
{
  return buildAsClaytonCopula(parameters).clone();
}

Distribution ClaytonCopulaFactory::build() const
{
  return buildAsClaytonCopula().clone();
}

ClaytonCopula ClaytonCopulaFactory::buildAsClaytonCopula(const Sample & sample) const
{
  if (sample.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a ClaytonCopula distribution from an empty sample";
  if (sample.getDimension() != 2) throw InvalidArgumentException(HERE) << "Error: cannot build a ClaytonCopula distribution from a sample of dimension not equal to 2";
  Scalar tau = sample.computeKendallTau().operator()(0, 1);
  if (tau == 1) throw InvalidArgumentException(HERE) << "Error: cannot build a ClaytonCopula distribution from a sample with Kendall tau equal to 1";
  ClaytonCopula result(2.0 * tau / (1.0 - tau));
  result.setDescription(sample.getDescription());
  return result;
}

ClaytonCopula ClaytonCopulaFactory::buildAsClaytonCopula(const Point & parameters) const
{
  try
  {
    ClaytonCopula copula;
    copula.setParameter(parameters);
    return copula;
  }
  catch (const InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a ClaytonCopula from the given parameters";
  }
}

ClaytonCopula ClaytonCopulaFactory::buildAsClaytonCopula() const
{
  return ClaytonCopula();
}

END_NAMESPACE_OPENTURNS

//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for Discrete distributions
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include <cstdlib>

#include "openturns/DiscreteDistribution.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(DiscreteDistribution)

/* Default constructor */
DiscreteDistribution::DiscreteDistribution()
  : DistributionImplementation()
  , supportEpsilon_(ResourceMap::GetAsScalar("DiscreteDistribution-SupportEpsilon"))
{
  // Nothing to do
}

/* Virtual constructor */
DiscreteDistribution * DiscreteDistribution::clone() const
{
  return new DiscreteDistribution(*this);
}

/* Comparison operator */
Bool DiscreteDistribution::operator ==(const DiscreteDistribution & other) const
{
  if (this == &other) return true;
  return false;
}

/* String converter */
String DiscreteDistribution::__repr__() const
{
  OSS oss;
  oss << "class=" << DiscreteDistribution::GetClassName();
  return oss;
}

/* Tell if the distribution is continuous */
Bool DiscreteDistribution::isContinuous() const
{
  return false;
}

/* Tell if the distribution is discrete */
Bool DiscreteDistribution::isDiscrete() const
{
  return true;
}

/* Tell if the distribution is integer valued */
Bool DiscreteDistribution::isIntegral() const
{
  return true;
}

/* Support tolerance accessor */
void DiscreteDistribution::setSupportEpsilon(const Scalar epsilon)
{
  supportEpsilon_ = epsilon;
}

Scalar DiscreteDistribution::getSupportEpsilon() const
{
  return supportEpsilon_;
}

/* Method save() stores the object through the StorageManager */
void DiscreteDistribution::save(Advocate & adv) const
{
  DistributionImplementation::save(adv);
  adv.saveAttribute( "supportEpsilon_", supportEpsilon_ );
}

/* Method load() reloads the object from the StorageManager */
void DiscreteDistribution::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  adv.loadAttribute( "supportEpsilon_", supportEpsilon_ );
}

END_NAMESPACE_OPENTURNS

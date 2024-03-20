//                                               -*- C++ -*-
/**
 *  @brief Results of distribution estimation
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
#include "openturns/DistributionFactoryResult.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS


CLASSNAMEINIT(DistributionFactoryResult)

static const Factory<DistributionFactoryResult> Factory_DistributionFactoryResult;

/* Default constructor */
DistributionFactoryResult::DistributionFactoryResult()
  : PersistentObject()
{
  // Nothing to do
}

/* Parameters constructor */
DistributionFactoryResult::DistributionFactoryResult(const Distribution & distribution,
    const Distribution & parameterDistribution)
  : PersistentObject()
  , distribution_(distribution)
  , parameterDistribution_(parameterDistribution)
{
  if (distribution_.getParameterDimension() != parameterDistribution.getDimension())
    throw InvalidDimensionException(HERE) << "The dimension of the parameter of the distribution (" << distribution_.getParameterDimension()
                                          << ") does not match the dimension of the distribution of the parameter (" << parameterDistribution.getDimension();
}

/* Virtual constructor */
DistributionFactoryResult * DistributionFactoryResult::clone() const
{
  return new DistributionFactoryResult(*this);
}

/* Accessor to the estimated distribution */
void DistributionFactoryResult::setDistribution(const Distribution & distribution)
{
  distribution_ = distribution;
}

Distribution DistributionFactoryResult::getDistribution() const
{
  return distribution_;
}

/* Accessor to the distribution of the parameter */
void DistributionFactoryResult::setParameterDistribution(const Distribution & parameterDistribution)
{
  parameterDistribution_ = parameterDistribution;
}

Distribution DistributionFactoryResult::getParameterDistribution() const
{
  return parameterDistribution_;
}

/* String converter */
String DistributionFactoryResult::__repr__() const
{
  OSS oss;
  oss << "class=" << getClassName()
      << " distribution=" << getDistribution()
      << " parameterDistribution=" << getParameterDistribution();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void DistributionFactoryResult::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("distribution_", distribution_);
  adv.saveAttribute("parameterDistribution_", parameterDistribution_);
}

/* Method load() reloads the object from the StorageManager */
void DistributionFactoryResult::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("distribution_", distribution_);
  adv.loadAttribute("parameterDistribution_", parameterDistribution_);
}


END_NAMESPACE_OPENTURNS

//                                               -*- C++ -*-
/**
 *  @brief Result of GEV profile likelihood
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
#include "openturns/DistributionFactoryLikelihoodResult.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(DistributionFactoryLikelihoodResult)

static const Factory<DistributionFactoryLikelihoodResult> Factory_DistributionFactoryLikelihoodResult;


DistributionFactoryLikelihoodResult::DistributionFactoryLikelihoodResult()
  : DistributionFactoryResult()
{}

DistributionFactoryLikelihoodResult::DistributionFactoryLikelihoodResult(const Distribution & distribution,
    const Distribution & parameterDistribution,
    const Scalar logLikelihood)
  : DistributionFactoryResult(distribution, parameterDistribution)
  , logLikelihood_(logLikelihood)
{}


DistributionFactoryLikelihoodResult * DistributionFactoryLikelihoodResult::clone() const
{
  return new DistributionFactoryLikelihoodResult(*this);
}

void DistributionFactoryLikelihoodResult::setLogLikelihood(const Scalar logLikelihood)
{
  logLikelihood_ = logLikelihood;
}

Scalar DistributionFactoryLikelihoodResult::getLogLikelihood() const
{
  return logLikelihood_;
}

String DistributionFactoryLikelihoodResult::__repr__() const
{
  return OSS() << DistributionFactoryResult::__repr__()
         << " logLikelihood_=" << logLikelihood_;
}

/* Method save() stores the object through the StorageManager */
void DistributionFactoryLikelihoodResult::save(Advocate & adv) const
{
  DistributionFactoryResult::save(adv);
  adv.saveAttribute("logLikelihood_", logLikelihood_);
}

/* Method load() reloads the object from the StorageManager */
void DistributionFactoryLikelihoodResult::load(Advocate & adv)
{
  DistributionFactoryResult::load(adv);
  adv.loadAttribute("logLikelihood_", logLikelihood_);
}

END_NAMESPACE_OPENTURNS

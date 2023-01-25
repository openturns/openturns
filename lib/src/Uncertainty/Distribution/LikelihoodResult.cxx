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
#include "openturns/LikelihoodResult.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(LikelihoodResult)

static const Factory<LikelihoodResult> Factory_LikelihoodResult;


LikelihoodResult::LikelihoodResult()
 : DistributionFactoryResult()
 {}

LikelihoodResult::LikelihoodResult(const Distribution & distribution,
                                   const Distribution & parameterDistribution,
                                   const Scalar logLikelihood)
 : DistributionFactoryResult(distribution, parameterDistribution)
 , logLikelihood_(logLikelihood)
 {}
 

LikelihoodResult * LikelihoodResult::clone() const
{
  return new LikelihoodResult(*this);
}

void LikelihoodResult::setLogLikelihood(const Scalar logLikelihood)
{
  logLikelihood_ = logLikelihood;
}

Scalar LikelihoodResult::getLogLikelihood() const
{
  return logLikelihood_;
}

String LikelihoodResult::__repr__() const
{
  return OSS() << DistributionFactoryResult::__repr__()
    << " logLikelihood_=" << logLikelihood_;
}

/* Method save() stores the object through the StorageManager */
void LikelihoodResult::save(Advocate & adv) const
{
  DistributionFactoryResult::save(adv);
  adv.saveAttribute("logLikelihood_", logLikelihood_);
}

/* Method load() reloads the object from the StorageManager */
void LikelihoodResult::load(Advocate & adv)
{
  DistributionFactoryResult::load(adv);
  adv.loadAttribute("logLikelihood_", logLikelihood_);
}

END_NAMESPACE_OPENTURNS

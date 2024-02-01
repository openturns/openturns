//                                               -*- C++ -*-
/**
 *  @brief Result of GEV profile likelihood
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
#ifndef OPENTURNS_DISTRIBUTIONFACTORYLIKELIHOODRESULT_HXX
#define OPENTURNS_DISTRIBUTIONFACTORYLIKELIHOODRESULT_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DistributionFactoryResult.hxx"

BEGIN_NAMESPACE_OPENTURNS

class OT_API DistributionFactoryLikelihoodResult
  : public DistributionFactoryResult
{
  CLASSNAME
public:
  DistributionFactoryLikelihoodResult();

  DistributionFactoryLikelihoodResult(const Distribution & distribution,
                                      const Distribution & parameterDistribution,
                                      const Scalar logLikelihood);

  DistributionFactoryLikelihoodResult * clone() const override;

  void setLogLikelihood(const Scalar logLikelihood);
  Scalar getLogLikelihood() const;

  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:
  Scalar logLikelihood_ = 0.0;
};

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DISTRIBUTIONFACTORYLIKELIHOODRESULT_HXX */

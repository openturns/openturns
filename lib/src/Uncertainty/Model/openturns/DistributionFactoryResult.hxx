//                                               -*- C++ -*-
/**
 *  @brief Results of distribution estimation
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
#ifndef OPENTURNS_DISTRIBUTIONFACTORYRESULT_HXX
#define OPENTURNS_DISTRIBUTIONFACTORYRESULT_HXX

#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class DistributionFactoryResult
 *
 */
class OT_API DistributionFactoryResult
  : public PersistentObject
{
  CLASSNAME
public:

  /** Default constructor */
  DistributionFactoryResult();

  /** Parameters constructor */
  DistributionFactoryResult(const Distribution & distribution,
                            const Distribution & parameterDistribution);

  /** Virtual constructor */
  DistributionFactoryResult * clone() const override;

  /** Accessor to the estimated distribution */
  void setDistribution(const Distribution & distribution);
  Distribution getDistribution() const;

  /** Accessor to the distribution of the parameter */
  void setParameterDistribution(const Distribution & parameterDistribution);
  Distribution getParameterDistribution() const;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;


protected:
  /** The estimated distribution */
  Distribution distribution_;

  /** The distribution of the parameter */
  Distribution parameterDistribution_;

}; /* class DistributionFactoryResult */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DISTRIBUTIONFACTORYRESULT_HXX */

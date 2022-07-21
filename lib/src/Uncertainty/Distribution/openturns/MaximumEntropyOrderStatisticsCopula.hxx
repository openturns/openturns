//                                               -*- C++ -*-
/**
 *  @brief The maximum entropy order statistics copula
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_MAXIMUMENTROPYORDERSTATISTICSCOPULA_HXX
#define OPENTURNS_MAXIMUMENTROPYORDERSTATISTICSCOPULA_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/SklarCopula.hxx"
#include "openturns/MaximumEntropyOrderStatisticsDistribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class MaximumEntropyOrderStatisticsCopula
 *
 * The maximum entropy order statistics copula
 */
class OT_API MaximumEntropyOrderStatisticsCopula
  : public SklarCopula
{
  CLASSNAME
public:

  typedef MaximumEntropyOrderStatisticsDistribution::DistributionCollection DistributionCollection;

  /** Default constructor */
  MaximumEntropyOrderStatisticsCopula();

  /** Parameters constructor */
  explicit MaximumEntropyOrderStatisticsCopula(const DistributionCollection & coll);

  /** Constructor with no check of the parameters, to speed-up marginal creations */
  MaximumEntropyOrderStatisticsCopula(const MaximumEntropyOrderStatisticsDistribution & distribution);

  /** Comparison operator */
  Bool operator ==(const MaximumEntropyOrderStatisticsCopula & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Distribution collection accessor */
  void setDistributionCollection(const DistributionCollection & coll);
  DistributionCollection getDistributionCollection() const;

  /* Interface inherited from Distribution */

  /** Virtual constructor */
  MaximumEntropyOrderStatisticsCopula * clone() const override;

  /** Get the kth approximation */
  PiecewiseHermiteEvaluation getApproximation(const UnsignedInteger k = 0) const;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  using SklarCopula::getMarginal;
  Distribution getMarginal(const Indices & indices) const override;

  /** Parameters value and description accessor */
  PointWithDescriptionCollection getParametersCollection() const override;
  using SklarCopula::setParametersCollection;
  void setParametersCollection(const PointCollection & parametersCollection) override;

  /* Interface specific to MaximumEntropyOrderStatisticsCopula */

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:


private:

  /** The underlying order statistics distribution */
  MaximumEntropyOrderStatisticsDistribution maxEntropyDistribution_;

}; /* class MaximumEntropyOrderStatisticsCopula */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MAXIMUMENTROPYORDERSTATISTICSCOPULA_HXX */

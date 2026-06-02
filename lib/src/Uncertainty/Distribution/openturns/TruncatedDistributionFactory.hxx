//                                               -*- C++ -*-
/**
 *  @brief Factory for truncated distributions
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_TRUNCATEDDISTRIBUTIONFACTORY_HXX
#define OPENTURNS_TRUNCATEDDISTRIBUTIONFACTORY_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DistributionFactoryImplementation.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/TruncatedDistribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class TruncatedDistributionFactory
 */
class OT_API TruncatedDistributionFactory
  : public DistributionFactoryImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  TruncatedDistributionFactory();

  /** Parameters constructor */
  explicit TruncatedDistributionFactory(const Distribution & distribution);

  /** Virtual constructor */
  TruncatedDistributionFactory * clone() const override;

  /* Here is the interface that all derived class must implement */
  using DistributionFactoryImplementation::build;

  Distribution build(const Sample & sample) const override;

  TruncatedDistribution buildAsTruncatedDistribution(const Sample & sample) const;

  /** Distribution accessor */
  void setDistribution(const Distribution & distribution);
  Distribution getDistribution() const;

  /** Estimate parameters flag accessor */
  void setEstimateParameters(const Bool estimateParameters);
  Bool getEstimateParameters() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

  /** The base distribution */
  Distribution distribution_;

  /** Whether to estimate the underlying distribution parameters */
  Bool estimateParameters_ = true;

}; /* class TruncatedDistributionFactory */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_TRUNCATEDDISTRIBUTIONFACTORY_HXX */

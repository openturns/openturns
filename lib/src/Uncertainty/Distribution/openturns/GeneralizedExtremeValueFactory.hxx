//                                               -*- C++ -*-
/**
 *  @brief Factory for DerivativeGeneralizedExtremeValue distribution
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
#ifndef OPENTURNS_GENERALIZEDEXTREMEVALUEFACTORY_HXX
#define OPENTURNS_GENERALIZEDEXTREMEVALUEFACTORY_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DistributionFactoryImplementation.hxx"
#include "openturns/GeneralizedExtremeValue.hxx"
#include "openturns/ProfileLikelihoodResult.hxx"
#include "openturns/Basis.hxx"
#include "openturns/TimeVaryingResult.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class GeneralizedExtremeValueFactory
 */
class OT_API GeneralizedExtremeValueFactory
  : public DistributionFactoryImplementation
{
  CLASSNAME
public:
  typedef Collection<Basis> BasisCollection;

  /** Default constructor */
  GeneralizedExtremeValueFactory();

  /** Virtual constructor */
  GeneralizedExtremeValueFactory * clone() const override;

  /* Here is the interface that all derived class must implement */
  using DistributionFactoryImplementation::build;

  Distribution build(const Sample & sample) const override;
  Distribution build(const Point & parameters) const override;
  Distribution build() const override;

  using DistributionFactoryImplementation::buildEstimator;
  DistributionFactoryResult buildEstimator(const Sample & sample) const override;

  GeneralizedExtremeValue buildAsGeneralizedExtremeValue(const Sample & sample) const;
  GeneralizedExtremeValue buildAsGeneralizedExtremeValue(const Point & parameters) const;
  GeneralizedExtremeValue buildAsGeneralizedExtremeValue() const;

  /** Maximum likelihood */
  GeneralizedExtremeValue buildMethodOfLikelihoodMaximization(const Sample & sample) const;
  LikelihoodResult buildMethodOfLikelihoodMaximizationEstimator(const Sample & sample) const;

  /** Profiled maximum likelihood */
  ProfileLikelihoodResult buildMethodOfProfileLikelihoodMaximizationEstimator(const Sample & sample) const;
  GeneralizedExtremeValue buildMethodOfProfileLikelihoodMaximization(const Sample & sample) const;

  /** R largest order statistics */
  DistributionFactoryResult buildRMaximaEstimator(const Sample & sample, const UnsignedInteger r = 0);
  GeneralizedExtremeValue buildRMaxima(const Sample & sample, const UnsignedInteger r = 0);
  UnsignedInteger buildBestRMaxima(const Sample & sample, const Indices & r, Point & logLikelihoodOut);

  /** Time-varying */
  TimeVaryingResult buildTimeVarying(const Sample & sample,
                                             const Mesh & mesh,
                                             const BasisCollection & basisCollection,
                                             const Function & inverseLinkFunction = Function()) const;

  /** Return level */
  Distribution buildReturnLevelEstimator(const DistributionFactoryResult & result, const Scalar m) const;

}; /* class GeneralizedExtremeValueFactory */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_GENERALIZEDEXTREMEVALUEFACTORY_HXX */

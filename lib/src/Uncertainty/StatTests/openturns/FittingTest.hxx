//                                               -*- C++ -*-
/**
 *  @brief StatTest implements statistical tests
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_FITTINGTEST_HXX
#define OPENTURNS_FITTINGTEST_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/TestResult.hxx"
#include "openturns/Collection.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/DistributionFactory.hxx"
#include "openturns/Pointer.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class FittingTest
 *
 */

class OT_API FittingTest
{
public:

  typedef Collection<DistributionFactory> DistributionFactoryCollection;
  typedef Collection<Distribution>        DistributionCollection;

  /** Best model for a given numerical sample by AIC */
  static Distribution BestModelAIC(const Sample &sample,
                                   const DistributionFactoryCollection &factoryCollection,
                                   Scalar &bestAICOut);

  /** Best model for a given numerical sample by AIC */
  static Distribution BestModelAIC(const Sample &sample,
                                   const DistributionCollection &distributionCollection,
                                   Scalar &bestAICOut);

  /** Best model for a given numerical sample by AICc */
  static Distribution BestModelAICC(const Sample &sample,
                                    const DistributionFactoryCollection &factoryCollection,
                                    Scalar &bestAICOut);

  /** Best model for a given numerical sample by AICc */
  static Distribution BestModelAICC(const Sample &sample,
                                    const DistributionCollection &distributionCollection,
                                    Scalar &bestAICOut);

  /** Best model for a given numerical sample by BIC */
  static Distribution BestModelBIC(const Sample  & sample,
                                   const DistributionFactoryCollection & factoryCollection,
                                   Scalar & bestBICOut);

  /** Best model for a given numerical sample by BIC */
  static Distribution BestModelBIC(const Sample  & sample,
                                   const DistributionCollection & distributionCollection,
                                   Scalar & bestBICOut);

  /** @deprecated Best model for a given numerical sample by Kolmogorov */
  static Distribution BestModelKolmogorov(const Sample  & sample,
                                          const DistributionFactoryCollection & factoryCollection,
                                          TestResult & bestResultOut);

  /** Best model for a given numerical sample by Lilliefors */
  static Distribution BestModelLilliefors(const Sample  & sample,
                                          const DistributionFactoryCollection & factoryCollection,
                                          TestResult & bestResultOut);

  /** Best model for a given numerical sample by Kolmogorov */
  static Distribution BestModelKolmogorov(const Sample  & sample,
                                          const DistributionCollection & distributionCollection,
                                          TestResult & bestResultOut);

  /** Best model for a given numerical sample by ChiSquared */
  static Distribution BestModelChiSquared(const Sample  & sample,
                                          const DistributionFactoryCollection & factoryCollection,
                                          TestResult & bestResultOut);

  /** Best model for a given numerical sample by ChiSquared */
  static Distribution BestModelChiSquared(const Sample  & sample,
                                          const DistributionCollection & distributionCollection,
                                          TestResult & bestResultOut);

  /** Akaike Information Criterion computation */
  static Scalar AIC(const Sample &sample,
                    const Distribution &distribution,
                    const UnsignedInteger estimatedParameters = 0);

  /** Akaike Information Criterion computation */
  static Distribution AIC(const Sample &sample,
                          const DistributionFactory &factory,
                          Scalar &bestAICOut);

  /** Akaike Information Criterion corrected (AICc) computation */
  static Scalar AICC(const Sample &sample,
                     const Distribution &distribution,
                     const UnsignedInteger estimatedParameters = 0);

  /** Akaike Information Criterion corrected (AICc) computation */
  static Distribution AICC(const Sample &sample,
                           const DistributionFactory &factory,
                           Scalar &bestAICOut);

  /** Bayesian Information Criterion computation */
  static Scalar BIC(const Sample & sample,
                    const Distribution & distribution,
                    const UnsignedInteger estimatedParameters = 0);

  /** Bayesian Information Criterion computation */
  static Distribution BIC(const Sample & sample,
                          const DistributionFactory & factory,
                          Scalar & bestBICOut);

  /** Kolmogorov fitting test for continuous distributions */
  static TestResult Kolmogorov(const Sample & sample,
                               const Distribution & distribution,
                               const Scalar level = 0.05);

  /** @deprecated Kolmogorov fitting test for continuous distributions */
  static TestResult Kolmogorov(const Sample & sample,
                               const DistributionFactory & factory,
                               Distribution & estimatedDistribution,
                               const Scalar level = 0.05);

  /** Lilliefors fitting test for continuous distributions */
  static TestResult Lilliefors(const Sample & sample,
                               const DistributionFactory & factory,
                               Distribution & estimatedDistribution,
                               const Scalar level = 0.05);

  /** Kolmogorov statistics computation */
  static Scalar ComputeKolmogorovStatistics(const Sample & sample,
      const Distribution & distribution);

  /** ChiSquared fitting test for discrete distributions */
  static TestResult ChiSquared(const Sample & sample,
                               const Distribution & distribution,
                               const Scalar level = 0.05,
                               const UnsignedInteger estimatedParameters = 0);

  /** ChiSquared fitting test for discrete distributions */
  static TestResult ChiSquared(const Sample & sample,
                               const DistributionFactory & factory,
                               Distribution & estimatedDistribution,
                               const Scalar level = 0.05);

private:
  FittingTest();

}; /* class FittingTest */

END_NAMESPACE_OPENTURNS
#endif /* OPENTURNS_FITTINGTEST_HXX */

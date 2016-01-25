//                                               -*- C++ -*-
/**
 *  @brief StatTest implements statistical tests
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OPENTURNS_FITTINGTEST_HXX
#define OPENTURNS_FITTINGTEST_HXX

#include "OTprivate.hxx"
#include "TestResult.hxx"
#include "Collection.hxx"
#include "NumericalSample.hxx"
#include "Distribution.hxx"
#include "DistributionFactory.hxx"
#include "Pointer.hxx"

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

  /** Best model for a given numerical sample by BIC */
  static Distribution BestModelBIC(const NumericalSample  & sample,
                                   const DistributionFactoryCollection & factoryCollection);

  /** Best model for a given numerical sample by BIC */
  static Distribution BestModelBIC(const NumericalSample  & sample,
                                   const DistributionCollection & distributionCollection);

  /** Best model for a given numerical sample by Kolmogorov */
  static Distribution BestModelKolmogorov(const NumericalSample  & sample,
                                          const DistributionFactoryCollection & factoryCollection,
                                          TestResult & bestResult);

  /** Best model for a given numerical sample by Kolmogorov */
  static Distribution BestModelKolmogorov(const NumericalSample  & sample,
                                          const DistributionCollection & distributionCollection,
                                          TestResult & bestResult);

  /** Best model for a given numerical sample by ChiSquared */
  static Distribution BestModelChiSquared(const NumericalSample  & sample,
                                          const DistributionFactoryCollection & factoryCollection,
                                          TestResult & bestResult);

  /** Best model for a given numerical sample by ChiSquared */
  static Distribution BestModelChiSquared(const NumericalSample  & sample,
                                          const DistributionCollection & distributionCollection,
                                          TestResult & bestResult);

  /** Bayesian Information Criterion computation */
  static NumericalScalar BIC(const NumericalSample & sample,
                             const Distribution & distribution,
                             const UnsignedInteger estimatedParameters = 0);

  /** Bayesian Information Criterion computation */
  static NumericalScalar BIC(const NumericalSample & sample,
                             const DistributionFactory & factory);

  /** Kolmogorov fitting test for continuous distributions */
  static TestResult Kolmogorov(const NumericalSample & sample,
                               const Distribution & distribution,
                               const NumericalScalar level = 0.95,
                               const UnsignedInteger estimatedParameters = 0);

  /** Kolmogorov fitting test for continuous distributions */
  static TestResult Kolmogorov(const NumericalSample & sample,
                               const DistributionFactory & factory,
                               const NumericalScalar level = 0.95);

  /** ChiSquared fitting test for discrete distributions */
  static TestResult ChiSquared(const NumericalSample & sample,
                               const Distribution & distribution,
                               const NumericalScalar level = 0.95,
                               const UnsignedInteger estimatedParameters = 0);

  /** ChiSquared fitting test for discrete distributions */
  static TestResult ChiSquared(const NumericalSample & sample,
                               const DistributionFactory & factory,
                               const NumericalScalar level = 0.95);

protected:
  /** Generic invocation of a R script for testing a distribution against a sample */
  static TestResult RunRTest(const NumericalSample & sample,
                             const Distribution & distribution,
                             const NumericalScalar level,
                             const UnsignedInteger estimatedParameters,
                             const String & testName);

private:
  FittingTest();

}; /* class FittingTest */

END_NAMESPACE_OPENTURNS
#endif /* OPENTURNS_FITTINGTEST_HXX */

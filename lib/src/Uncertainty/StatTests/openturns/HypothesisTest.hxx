//                                               -*- C++ -*-
/**
 *  @brief StatTest implements statistical tests
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_HYPOTHESISTEST_HXX
#define OPENTURNS_HYPOTHESISTEST_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/TestResult.hxx"
#include "openturns/Indices.hxx"
#include "openturns/Sample.hxx"
#include "openturns/LinearModel.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/DistributionFactoryImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class HypothesisTest
 *
 */

class OT_API HypothesisTest
{
public:

  typedef Collection<TestResult>      TestResultCollection;

  /** Independance ChiSquared test between 2 scalar samples for discrete distributions */
  static TestResult ChiSquared(const Sample & firstSample,
                               const Sample & secondSample,
                               const Scalar level = 0.05);

  /** Independence Pearson test between 2 samples which form a gaussian vector: test the linear relation */
  static TestResult Pearson(const Sample & firstSample,
                            const Sample & secondSample,
                            const Scalar level = 0.05);

  /** @deprecated method : use TwoSamplesKolmogorov */
  /** Smirnov test if two scalar samples (of sizes not necessarily equal) follow the same distribution (only for continuous distributions) */
  static TestResult Smirnov(const Sample & firstSample,
                            const Sample & secondSample,
                            const Scalar level = 0.05);

  /** Two-sample Kolmogorov–Smirnov test */
  static TestResult TwoSamplesKolmogorov (const Sample & sample1,
                                          const Sample & sample2,
                                          const Scalar level = 0.05);

  /** Spearman test between 2 scalar samples : test the monotonous relation   */
  static TestResult Spearman(const Sample & firstSample,
                             const Sample & secondSample,
                             const Scalar level = 0.05);


  /** Independence Pearson test between 2 samples : firstSample of dimension n and secondSample of dimension 1. If firstSample[i] is the numeriacl sample extracted from firstSample (ith coordinate of each point of the numerical sample), PartialPearson performs the Independence Pearson test simultaneously on firstSample[i] and secondSample, for i in the selection. For all i, it is supposed that the couple (firstSample[i] and secondSample) is issued from a gaussian  vector. */
  static TestResultCollection PartialPearson(const Sample & firstSample,
      const Sample & secondSample,
      const Indices & selection,
      const Scalar level = 0.05);

  /** Regression test between 2 samples : firstSample of dimension n and secondSample of dimension 1. If firstSample[i] is the numerical sample extracted from firstSample (ith coordinate of each point of the numerical sample), PartialRegression performs the Regression test simultaneously on all firstSample[i] and secondSample, for i in the selection. The Regression test tests ifthe regression model between two scalar numerical samples is significant. It is based on the deviation analysis of the regression. The Fisher distribution is used. */
  static TestResultCollection PartialRegression(const Sample & firstSample,
      const Sample & secondSample,
      const Indices & selection,
      const Scalar level = 0.05);

  /** Spearman test between 2 samples : firstSample of dimension n and secondSample of dimension 1. If firstSample[i] is the numerical sample extracted from firstSample (ith coordinate of each point of the numerical sample), PartialSpearman performs the Independence Spearman test simultaneously on firstSample[i] and secondSample, for i in the selection.  */
  static TestResultCollection PartialSpearman(const Sample & firstSample,
      const Sample & secondSample,
      const Indices & selection,
      const Scalar level = 0.05);

  /** Independence Pearson test between 2 samples : firstSample of dimension n and secondSample of dimension 1. If firstSample[i] is the numerical sample extracted from firstSample (ith coordinate of each point of the numerical sample), FullPearson performs the Independence Pearson test simultaneously on all firstSample[i] and secondSample. For all i, it is supposed that the couple (firstSample[i] and secondSample) is issued from a gaussian  vector. */
  static TestResultCollection FullPearson(const Sample & firstSample,
                                          const Sample & secondSample,
                                          const Scalar level = 0.05);

  /** @deprecated, moved to LinearModelTest */
  static TestResultCollection FullRegression(const Sample & firstSample,
      const Sample & secondSample,
      const Scalar level = 0.05);

  /** Spearman test between 2 samples : firstSample of dimension n and secondSample of dimension 1. If firstSample[i] is the numerical sample extracted from firstSample (ith coordinate of each point of the numerical sample), PartialSpearman performs the Independence Spearman test simultaneously on all firstSample[i] and secondSample. */
  static TestResultCollection FullSpearman(const Sample & firstSample,
      const Sample & secondSample,
      const Scalar level = 0.05);

protected:

  HypothesisTest();
}; /* class HypothesisTest */

END_NAMESPACE_OPENTURNS
#endif /* OPENTURNS_HYPOTHESISTEST_HXX */

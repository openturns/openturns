//                                               -*- C++ -*-
/**
 *  @brief StatTest implements statistical tests
 *  This statistical test enables user to check if a time series is stationary
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
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/DickeyFullerTest.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Point.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/IdentityMatrix.hxx"
#include "openturns/FisherSnedecor.hxx"
#include "openturns/Log.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(DickeyFullerTest)
static const Factory<DickeyFullerTest> Factory_DickeyFullerTest;


/* Default constructor */
DickeyFullerTest::DickeyFullerTest()
  : PersistentObject()
  , timeSeries_()
  , T_()
  , verbose_(false)
  , lastModel_(0)
  , rho_(0.0)
  , sigmaRho_(1.0)
  , drift_(0.0)
  , sigmaDrift_(1.0)
  , trend_(0.0)
  , sigmaTrend_(1.0)
  , isComputedNoConstantCharacteristics_(false)
  , sum_yt_yt_minus_one_(0.0)
  , sum_squared_yt_minus_one_(0.0)
  , isComputedDriftCharacteristics_(false)
  , sum_yt_minus_one_(0.0)
  , sum_yt_(0.0)
  , isComputedTrendCharacteristics_(false)
  , sum_t_yt_minus_one_(0.0)
  , sum_t_yt_(0.0)
  , sum_ti_(0.0)
  , sum_ti_ti_(0.0)
{
  // Nothing to do
}

/* Parameters constructor */
DickeyFullerTest::DickeyFullerTest(const TimeSeries & series,
                                   const Bool verbose)
  : PersistentObject()
  , timeSeries_(series)
  , T_()
  , verbose_(verbose)
  , lastModel_()
  , rho_(0.0)
  , sigmaRho_(1.0)
  , drift_(0.0)
  , sigmaDrift_(1.0)
  , trend_(0.0)
  , sigmaTrend_(1.0)
  , isComputedNoConstantCharacteristics_(false)
  , sum_yt_yt_minus_one_(0.0)
  , sum_squared_yt_minus_one_(0.0)
  , isComputedDriftCharacteristics_(false)
  , sum_yt_minus_one_(0.0)
  , sum_yt_(0.0), isComputedTrendCharacteristics_(false)
  , sum_t_yt_minus_one_(0.0)
  , sum_t_yt_(0.0)
  , sum_ti_(0.0)
  , sum_ti_ti_(0.0)
{
  if (series.getOutputDimension() != 1)
    throw InvalidDimensionException(HERE) << "Expected time series of dimension 1";

  // Build the estimation of parameters rho and \phi_i
  const UnsignedInteger size = series.getSize();
  if (size <= 1)
    throw InvalidArgumentException(HERE) << "The size of time series is " << size
                                         << "Impossible to run the test" ;

  T_ = size - 1;
}

/* Virtual constructor */
DickeyFullerTest * DickeyFullerTest::clone() const
{
  return new DickeyFullerTest(*this);
}

/* Computation of \sum_{t=1}^{N} y_{t} y{t-1} and \sum_{t=1}^{N} y_{t-1} * y{t-1} */
void DickeyFullerTest::computeNoConstantCharacteristics()
{
  if (!isComputedNoConstantCharacteristics_)
  {
    // Computation of \sum_{t=1}^{N} y_{t} y{t-1} and
    // Computation of \sum_{t=1}^{N} y_{t-1} * y{t-1}
    for (UnsignedInteger t = 0; t < T_; ++t)
    {
      const Scalar y_t_minus_one = timeSeries_.getValueAtIndex(t)[0];
      const Scalar y_t = timeSeries_.getValueAtIndex(t + 1)[0];
      sum_yt_yt_minus_one_ += y_t_minus_one * y_t;
      sum_squared_yt_minus_one_ += y_t_minus_one * y_t_minus_one;
    }
    isComputedNoConstantCharacteristics_ = true;
  }
}

/* Computation of:
   1) \sum_{t=1}^{N} y{t-1}
   2) \sum_{t=1}^{N} y{t}
*/
void DickeyFullerTest::computeDriftCharacteristics()
{
  if (!isComputedDriftCharacteristics_)
  {
    Scalar partialSum = 0.0;
    for (UnsignedInteger t = 1; t < T_; ++t) partialSum += timeSeries_.getValueAtIndex(t)[0];
    sum_yt_minus_one_ = partialSum + timeSeries_.getValueAtIndex(0)[0];
    sum_yt_ = partialSum + timeSeries_.getValueAtIndex(T_)[0];
    isComputedDriftCharacteristics_ = true;
  }
}

/* Computation of:
   1) \sum_{t=1}^{N} t * y{t-1}
   2) \sum_{t=1}^{N} t * y{t}
   3) \sum_{i=1}^{N} t_{i}
   4) \sum_{i=1}^{N} t_{i}^2
*/
void DickeyFullerTest::computeTrendCharacteristics()
{
  RegularGrid timeGrid(timeSeries_.getTimeGrid());
  const Scalar dt = timeGrid.getStep();
  const Scalar t0 = timeGrid.getStart();
  if (!isComputedTrendCharacteristics_)
  {
    // Computation of \sum_{t=1}^{N} t * y{t-1}
    // and \sum_{t=1}^{N} t * y{t}
    for (UnsignedInteger t = 0; t < T_; ++t)
    {
      const Scalar y_t_minus_one = timeSeries_.getValueAtIndex(t)[0];
      const Scalar time = timeGrid.getValue(t + 1);
      const Scalar y_t = timeSeries_.getValueAtIndex(t + 1)[0];
      sum_t_yt_minus_one_ += y_t_minus_one * time;
      sum_t_yt_ += y_t * time;
    }
    // Computation of \sum_{i=1}^{N} t_{i} and \sum_{i=1}^{N} t_{i}^2
    // We recall that t_{i} = t_{start} + i * t_{step}
    // Thus  \sum_{i=1}^{T_} t_{i} = t_{start} + 0.5 * dt * (T_ + 1) and
    // Thus  \sum_{i=1}^{T_} t_{i}^2 = t_{start} * t_{start} + t_{start} * dt * (T_ + 1) + dt * dt * (T_ + 1) * (2 * T_ + 1) /6
    sum_ti_ = T_ * (t0 + 0.5 * dt * (T_ + 1.0));
    sum_ti_ti_ = T_ * (t0 * t0 + t0 * dt * (T_ + 1.0) + dt * dt * (T_ + 1.0) * (2.0 * T_ + 1.0) / 6.0);
    isComputedTrendCharacteristics_ = true;
  }
}


/* Compute the coefficients of a model with trend and constant */
void DickeyFullerTest::estimateDriftAndLinearTrendModel()
{
  // Linear system (of dimension 3) is solved
  // We recall the analytical expression of the matrix
  // The unknowns are sorted as following : drift, trend and AR coefficient
  // i.e. model is y_{t} = a + b *t + \rho * y_{t-1} + \epsilon_t
  // We could refer to classical linear algebra documentation to get the expression of the matrix
  // which is [1 \sum_t_{i} \sum_y_{t-1}; \sum_t_{i} \sum_t_{i}^2 \sum_t_{i} y_{i-1}; \sum_y_{t-1} \sum_y_{t-1} t_{i} \sum_y_{t-1}*y_{t-1}]
  // Some expressions such as \sum t_{i} or \sum t_{i}^2 may be easily deduced using the fact that t_i = t_0 + i * \delta t
  // Second member is [\sum_y_{t} \sum_t_{i} * y_{i} \sum y_{t-1} y_{t}]
  computeNoConstantCharacteristics();
  computeDriftCharacteristics();
  computeTrendCharacteristics();

  if (lastModel_ != 3)
  {
    SymmetricMatrix matrix(3);

    matrix(0, 0) = T_;
    matrix(1, 0) = sum_ti_;
    matrix(2, 0) = sum_yt_minus_one_;
    matrix(1, 1) = sum_ti_ti_;
    matrix(2, 1) = sum_t_yt_minus_one_;
    matrix(2, 2) = sum_squared_yt_minus_one_;


    Point secondMember(3);
    secondMember[0] = sum_yt_;
    secondMember[1] = sum_t_yt_;
    secondMember[2] = sum_yt_yt_minus_one_;

    // Solving the linear system
    Point unknown(matrix.solveLinearSystem(secondMember));

    drift_ = unknown[0];
    trend_ = unknown[1];
    rho_ = unknown[2];

    // Estimate the sum square of residual value
    Scalar error = 0.0;
    const RegularGrid timeGrid_(timeSeries_.getTimeGrid());
    for (UnsignedInteger t = 0; t < T_; ++t)
    {
      const Scalar x = timeSeries_.getValueAtIndex(t)[0];
      const Scalar y = timeSeries_.getValueAtIndex(t + 1)[0];
      const Scalar time = timeGrid_.getValue(t + 1);
      const Scalar epsilon = y - rho_ * x - trend_ * time - drift_;
      error += epsilon * epsilon;
    }
    const Scalar sigmaError = sqrt(error / T_);
    // Estimate the variance-covariance matrix associated to the coefficient
    const IdentityMatrix identity(3);
    const Matrix varianceCovariance(matrix.solveLinearSystem(identity));

    sigmaDrift_ = sigmaError * sqrt(varianceCovariance(0, 0));
    sigmaTrend_ = sigmaError * sqrt(varianceCovariance(1, 1));
    sigmaRho_ = sigmaError * sqrt(varianceCovariance(2, 2));
  }

  // inform that the last model which has been checked is the trend model
  lastModel_ = 3;
}

/* Compute the coefficients of a model with a constant */
void DickeyFullerTest::estimateDriftModel()
{
  if (lastModel_ != 2)
  {
    trend_ = 0.0;
    sigmaTrend_ = 0.0;
    computeNoConstantCharacteristics();
    computeDriftCharacteristics();
    // Linear system of dimension 2
    // The size term vanish since we use a biased variance estimator
    SymmetricMatrix matrix(2);

    matrix(0, 0) = T_;
    matrix(1, 0) = sum_yt_minus_one_;
    matrix(1, 1) = sum_squared_yt_minus_one_;


    Point secondMember(2);
    secondMember[0] = sum_yt_;
    secondMember[1] = sum_yt_yt_minus_one_;

    // Solving the linear system
    Point unknown(matrix.solveLinearSystem(secondMember));

    drift_ = unknown[0];
    rho_ = unknown[1];

    // Estimate the sum square of residual value
    Scalar error = 0.0;
    for (UnsignedInteger t = 0; t < T_; ++ t)
    {
      const Scalar x = timeSeries_.getValueAtIndex(t)[0];
      const Scalar y = timeSeries_.getValueAtIndex(t + 1)[0];
      const Scalar epsilon = y - rho_ * x - drift_;
      error += epsilon * epsilon;
    }
    const Scalar sigmaError = sqrt(error / T_);

    // Estimate the variance-covariance matrix associated to the coefficient
    const IdentityMatrix identity(2);
    const Matrix varianceCovariance(matrix.solveLinearSystem(identity));

    sigmaDrift_ = sigmaError * sqrt(varianceCovariance(0, 0));
    sigmaRho_ = sigmaError * sqrt(varianceCovariance(1, 1));
  }
  lastModel_ = 2;
}

/* Compute the coefficients of a model with a constant */
void DickeyFullerTest::estimateAR1Model()
{
  if (lastModel_ != 1)
  {
    trend_ = 0.0;
    sigmaTrend_ = 0.0;
    drift_ = 0.0;
    sigmaDrift_ = 0.0;

    computeNoConstantCharacteristics();
    // The rho value is the simplest ratio
    rho_ = sum_yt_yt_minus_one_ / sum_squared_yt_minus_one_;

    // Expected error residual
    Scalar error = 0.0;
    for (UnsignedInteger t = 0; t < T_; ++ t)
    {
      const Scalar x = timeSeries_.getValueAtIndex(t)[0];
      const Scalar y = timeSeries_.getValueAtIndex(t + 1)[0];
      const Scalar epsilon = y - rho_ * x;
      error += epsilon * epsilon;
    }
    // Writing the expression of the statistic
    const Scalar sigmaError = sqrt(error / T_);
    sigmaRho_ = sigmaError / sqrt(sum_squared_yt_minus_one_);
  }
  lastModel_ = 1;
}

/* Test H0: there is a unit root in a drift and linear trend model */
TestResult DickeyFullerTest::testUnitRootInDriftAndLinearTrendModel(const Scalar level)
{
  // compute the coefficients of trend model
  estimateDriftAndLinearTrendModel();
  const Scalar statistic ((rho_ - 1.0) / sigmaRho_);
  // Statistical test : the null hypothesis is that there is a unit root
  const Scalar pValue = DistFunc::pDickeyFullerTrend(statistic);
  return TestResult("DickeyFullerUnitRootInDriftAndLinearTrendModel", pValue > level, pValue, level, statistic);
}

/* Test H0: there is a unit root in a drift model */
TestResult DickeyFullerTest::testUnitRootInDriftModel(const Scalar level)
{
  // compute the coefficients of the model
  estimateDriftModel();
  // Writing the expression of the statistic
  const Scalar statistic = (rho_ - 1.0) / sigmaRho_;

  // We compare the statistic of the test with the critical value of the DF test
  Scalar pValue = DistFunc::pDickeyFullerConstant(statistic);
  return TestResult("DickeyFullerUnitRootInDriftModel", pValue > level, pValue, level, statistic);
}

/* Test H0: there is a unit root in an AR1 model */
TestResult DickeyFullerTest::testUnitRootInAR1Model(const Scalar level)
{
  estimateAR1Model();
  const Scalar statistic = (rho_ - 1.0) / sigmaRho_;
  // We get now the statistic of the test and compare it with the critical value of the DF test
  const Scalar pValue = DistFunc::pDickeyFullerNoConstant(statistic);
  return TestResult("DickeyFullerUnitRootAR1Model", pValue > level, pValue, level, statistic);
}


/* Strategy method to check the stationarity */
TestResult DickeyFullerTest::runStrategy(const Scalar level)
{
  // Run the trend model
  if (verbose_) LOGINFO("Running test with general model...\nTesting unit root...");
  const TestResult runTrendTestModel(testUnitRootInDriftAndLinearTrendModel(level));

  if (!runTrendTestModel.getBinaryQualityMeasure())
  {
    if (verbose_) LOGINFO("Testing unit root with general model rejected.\nRunning test of nullity of trend coefficient...");
    // Student test
    TestResult testNullTrend(testNoUnitRootAndNoLinearTrendInDriftAndLinearTrendModel(level));
    if (verbose_) LOGINFO("Test of nullity of trend coefficient done.");
    if (verbose_) LOGINFO(testNullTrend.__repr__());
    // Test is rejected if the the statistic is greater than the quantile
    if (!testNullTrend.getBinaryQualityMeasure())
    {
      if (verbose_) LOGINFO("Test of nullity of trend coefficient rejected ==> No unit root and trend stationary.");
      if (verbose_) LOGINFO("End of strategy tests");
      return testNullTrend;
    }
    else
    {
      if (verbose_) LOGINFO("Test of nullity of trend coefficient accepted.");
      return runDriftModelStrategyTest(level);
    }
  }
  else
  {
    if (verbose_) LOGINFO("Testing unit root with general model accepted.\nRunning test of nullity of trend coefficient and unit root");
    // Fisher test
    TestResult testNullTrend(testNoUnitRootAndNoLinearTrendInDriftAndLinearTrendModel(level));
    if (verbose_) LOGINFO("Test done.");
    if (verbose_) LOGINFO(testNullTrend.__repr__());

    // Test is rejected if the the statistic is greater than the quantile
    if (!testNullTrend.getBinaryQualityMeasure())
    {
      // Check the pValue ==> False
      // The test bases on the fact that H0 :
      if (verbose_) LOGINFO("Test of nullity of trend coefficient rejected ==> trend stationary and no unit root");
      if (verbose_) LOGINFO("End of strategy tests");
      return testNullTrend;
    }
    else
    {
      return runDriftModelStrategyTest(level);
    }
  }
}

/* Run strategy of tests with drift model */
TestResult DickeyFullerTest::runDriftModelStrategyTest(const Scalar level)
{
  // Run the trend model
  if (verbose_) LOGINFO("Running test with drift model...\nTesting unit root...");
  // First check the presence of a unit root in the drift model
  const TestResult runDriftTestModel(testUnitRootInDriftModel(level));

  if (!runDriftTestModel.getBinaryQualityMeasure())
  {
    if (verbose_) LOGINFO("Unit root test with drift model rejected.\nRunning test of nullity of drift coefficient...");
    // Student test
    TestResult testNullDrift(testNoUnitRootAndNoDriftInDriftModel(level));
    if (verbose_) LOGINFO("Test done.");
    if (verbose_) LOGINFO(testNullDrift.__repr__());
    // Test is rejected if the the statistic is greater than the quantile
    if (!testNullDrift.getBinaryQualityMeasure())
    {
      if (verbose_) LOGINFO("Test of nullity of drift coefficient rejected ==> No unit root but drift.");
      if (verbose_) LOGINFO("End of strategy tests");
      return testNullDrift;
    }
    else
    {
      if (verbose_) LOGINFO("Test of nullity of drift coefficient accepted. Checking a random walk");
      return runRandomWalkModelStrategyTest(level);
    }
  }
  else
  {
    if (verbose_) LOGINFO("Testing unit root with drift model accepted.\nRunning test of nullity of drift coefficient and unit root");
    // Fisher test
    TestResult testNullDrift(testNoUnitRootAndNoDriftInDriftModel(level));
    if (verbose_) LOGINFO("Test done.");
    if (verbose_) LOGINFO(testNullDrift.__repr__());

    // Test is rejected if the the statistic is greater than the quantile
    if (!testNullDrift.getBinaryQualityMeasure())
    {
      if (verbose_) LOGINFO("Test of nullity of drift coefficient rejected ==> drift and unit root");
      if (verbose_) LOGINFO("End of strategy tests");
      return testNullDrift;
    }
    else
    {
      if (verbose_) LOGINFO("Test of nullity of drift coefficient accepted. Checking a random walk");
      return runRandomWalkModelStrategyTest(level);
    }
  }
}

/* Test of random walk model ==> run a part of strategy */
TestResult DickeyFullerTest::runRandomWalkModelStrategyTest(const Scalar level)
{
  // Run the trend model
  if (verbose_) LOGINFO("Running test with random walk model...\nTesting unit root...");
  // First check the presence of a unit root in the drift model
  const TestResult runRWTestModel(testUnitRootInAR1Model(level));
  if (!runRWTestModel.getBinaryQualityMeasure())
  {
    if (verbose_) LOGINFO("Unit root test rejected. Stationary checked");
  }
  else
  {
    if (verbose_) LOGINFO("Unit root test accepted.");
  }

  if (verbose_) LOGINFO("End of strategy tests");
  return runRWTestModel;

}

/* Test H0: there is no linear trend in a drift and linear trend model knowing that there is a unit root
   Run a Fisher test
   H0 : Trend coefficient is null conditional to the acceptation of the fact that rho is 1
   H1 : Trend coefficient significatively different from 0
   Construction of the statistic
   The notations are similar to the Course of Non Stationary process - Dauphine
   SCR3c is the sum of square of residual using Trend model with conditions that rho_ = 1,  trend_ = 0
   i.e. x_{t} = x_{t} + c + \epsilon_t
   c is given as (x_{N-1} - x_{0}) / N - 1
*/
TestResult DickeyFullerTest::testUnitRootAndNoLinearTrendInDriftAndLinearTrendModel(const Scalar level)
{
  // We first compute the coefficients of the trend model
  TestResult resultTestTrendModel(testUnitRootInDriftAndLinearTrendModel(level));
  if (!resultTestTrendModel.getBinaryQualityMeasure())
  {
    LOGWARN("The model has no unit root. The result test may have no sense. The statistical \"testNoUnitRootAndNoLinearTrendInDriftAndLinearTrendModel\" test is suggested ");
  }

  const Scalar c = (timeSeries_(T_, 1) - timeSeries_(0, 1)) / T_;
  Scalar SCR3c = 0.0;
  Scalar SCR3 = 0.0;

  const RegularGrid timeGrid(timeSeries_.getTimeGrid());
  for (UnsignedInteger t = 0; t < T_; ++t)
  {
    const Scalar deltaX = timeSeries_(t + 1, 1) - timeSeries_(t, 1);
    const Scalar epsilon_t = deltaX - c;
    SCR3c += epsilon_t * epsilon_t;
    // Perform since it is computed previously
    const Scalar x = timeSeries_.getValueAtIndex(t)[0];
    const Scalar y = timeSeries_.getValueAtIndex(t + 1)[0];
    const Scalar tick = timeGrid.getValue(t + 1);
    const Scalar epsilon = y - rho_ * x - trend_ * tick - drift_;
    SCR3 += epsilon * epsilon;
  }

  // F3 statistic
  // Fisher statistic
  const Scalar fisherStatisticNullTrend = 0.5 * (SCR3c - SCR3) * (T_ -  2) / SCR3;
  // Fisher test
  const FisherSnedecor fisherSnedecor(2, T_ - 2);
  const Scalar pValueNullTrend = fisherSnedecor.computeCDF(fisherStatisticNullTrend);

  // Decision
  const TestResult result("DickeyFullerTrendNullWithUnitRoot", pValueNullTrend < level, pValueNullTrend, level, fisherStatisticNullTrend);
  return result;
}

/* Test H0: there is no linear trend in a drift and linear trend model knowing that there is no unit root
   Run a Student test
   H0 : Trend coefficient is null ==> trend_ = 0
   H1 : Trend coefficient significatively different from 0
*/
TestResult DickeyFullerTest::testNoUnitRootAndNoLinearTrendInDriftAndLinearTrendModel(const Scalar level)
{
  // First compute all coefficients of trend model
  TestResult resultTestTrendModel(testUnitRootInDriftAndLinearTrendModel(level));

  if (resultTestTrendModel.getBinaryQualityMeasure())
  {
    LOGWARN("The model has a unit root. The result test may have no sense. The statistical \"testNullTrendCoefficientWithUnitRoot\" test is suggested ");
  }

  // We write the statistic of studentized
  // Care: check that the variance is the non biased estimator
  // Student quantile ==> T(n - p - 1) with n: size of sample and p: number of variables
  // p here is 3 (rho, drift and trend)
  const Scalar studentStatisticNullTrend = std::abs(trend_) / sigmaTrend_;
  // Here, T_ = n - 1
  const Scalar complementaryCDFNullTrend = DistFunc::pStudent(T_ - 3, studentStatisticNullTrend, true);
  // True pValue = 1 - F(s) with s the student statistic and F the Student cumulative function

  // Test is rejected if the the statistic is greater than the quantile
  const TestResult result("DickeyFullerTrendNullWithoutUnitRoot", complementaryCDFNullTrend > level, complementaryCDFNullTrend, level, studentStatisticNullTrend);
  return result;

}

/* Test H0: there is no drift in a drift model knowing that there is a unit root
   Run a Fisher test
   H0 : Trend coefficient is null conditional to the acceptation of the fact that rho is 1
   H1 : Trend coefficient significatively different from 0
   Construction of the statistic
   The notations are similar to the Course of Non Stationary process - Dauphine
   SCR2c is the sum of square of residual using Trend model with conditions that rho_ = 1,  trend_ = 0
   i.e. x_{t} = x_{t} + \epsilon_t
   c is given as (x_{N-1} - x_{0}) / N - 1
*/
TestResult DickeyFullerTest::testUnitRootAndNoDriftInDriftModel(const Scalar level)
{

  // Run the computation of drift model
  TestResult resultTestDriftModel(testUnitRootInDriftModel(level));

  if (!resultTestDriftModel.getBinaryQualityMeasure())
  {
    LOGWARN("The model has no unit root. The result test may have no sense. The statistical \"testNoUnitRootAndNoDriftInDriftModel\" test is suggested ");
  }

  Scalar SCR2c = 0.0;
  Scalar SCR2 = 0.0;

  for (UnsignedInteger t = 0; t < T_; ++ t)
  {
    const Scalar epsilon_t = timeSeries_(t + 1, 1) - timeSeries_(t, 1);
    SCR2c += epsilon_t * epsilon_t;
    // Perform since it is computed previously
    const Scalar X = timeSeries_.getValueAtIndex(t)[0];
    const Scalar Y = timeSeries_.getValueAtIndex(t + 1)[0];
    const Scalar epsilon = Y - rho_ * X  - drift_;
    SCR2 += epsilon * epsilon;
  }

  // F2 statistic
  // Fisher statistic
  const Scalar fisherStatisticNullDrift = 0.5 * (SCR2c - SCR2) * (T_ -  1) / SCR2;
  // Fisher test
  const FisherSnedecor fisherSnedecor(2, T_ - 2);
  const Scalar pValueNullDrift = fisherSnedecor.computeCDF(fisherStatisticNullDrift);

  // Decision
  TestResult result("DickeyFullerDriftNullWithUnitRoot", pValueNullDrift < level, pValueNullDrift, level, fisherStatisticNullDrift);
  return result;

}

/* Test the nullity of drift coefficient with assumption that there is no unit root
   Run a Student test
   H0 : Drift coefficient is null
   H1 : Drift coefficient significatively different from 0
*/
TestResult DickeyFullerTest::testNoUnitRootAndNoDriftInDriftModel(const Scalar level)
{
  // Run the computation of drift model
  TestResult resultTestDriftModel(testUnitRootInDriftModel(level));

  if (resultTestDriftModel.getBinaryQualityMeasure())
  {
    LOGWARN("The model has a unit root. The result test may have no sense. The statistical \"testUnitRootAndNoDriftInDriftModel\" test is suggested ");
  }

  // Perform the statistical test
  const Scalar studentStatisticNullDrift = std::abs(drift_) / sigmaDrift_;
  // Student quantile ==> T(n - p - 1) with n: size of sample and p: number of variables
  const Scalar complementaryCDFNullDrift = DistFunc::pStudent(T_ - 2, studentStatisticNullDrift);

  // Test is rejected if the the statistic is greater than the quantile
  const TestResult result("DickeyFullerDriftNullWithoutUnitRoot", complementaryCDFNullDrift < level, complementaryCDFNullDrift, level, studentStatisticNullDrift);
  return result;

}

/* Verbosity set accessor */
void DickeyFullerTest::setVerbose(const Bool verbose)
{
  verbose_ = verbose;
}

/* Verbosity get accessor */
Bool DickeyFullerTest::getVerbose() const
{
  return verbose_;
}

/* Method save() stores the object through the StorageManager */
void DickeyFullerTest::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("timeSeries_", timeSeries_);
  adv.saveAttribute("T_", T_);
  adv.saveAttribute("verbose_", verbose_);
  adv.saveAttribute("sum_yt_yt_minus_one_", sum_yt_yt_minus_one_);
  adv.saveAttribute("sum_squared_yt_minus_one_", sum_squared_yt_minus_one_);
  adv.saveAttribute("lastModel_", lastModel_);
  adv.saveAttribute("rho_", rho_);
  adv.saveAttribute("sigmaRho_", sigmaRho_);
  adv.saveAttribute("isComputedNoConstantCharacteristics_", isComputedNoConstantCharacteristics_);
  adv.saveAttribute("sum_yt_minus_one_", sum_yt_minus_one_);
  adv.saveAttribute("sum_y_t_", sum_yt_);
  adv.saveAttribute("drift_", drift_);
  adv.saveAttribute("sigmaDrift_", sigmaDrift_);
  adv.saveAttribute("sum_t_yt_minus_one_", sum_t_yt_minus_one_);
  adv.saveAttribute("sum_t_yt_", sum_t_yt_);
  adv.saveAttribute("sum_ti_", sum_ti_);
  adv.saveAttribute("sum_ti_ti_", sum_ti_ti_);
  adv.saveAttribute("trend_", trend_);
  adv.saveAttribute("sigmaTrend_", sigmaTrend_);
  adv.saveAttribute("isComputedTrendCharacteristics_", isComputedTrendCharacteristics_);
}

/* Method load() reloads the object from the StorageManager */
void DickeyFullerTest::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("timeSeries_", timeSeries_ );
  adv.loadAttribute("T_", T_ );
  adv.loadAttribute("verbose_", verbose_ );
  adv.loadAttribute("sum_yt_yt_minus_one_", sum_yt_yt_minus_one_);
  adv.loadAttribute("sum_squared_yt_minus_one_", sum_squared_yt_minus_one_);
  adv.loadAttribute("lastModel_", lastModel_);
  adv.loadAttribute("rho_", rho_);
  adv.loadAttribute("sigmaRho_", sigmaRho_);
  adv.loadAttribute("isComputedNoConstantCharacteristics_", isComputedNoConstantCharacteristics_);
  adv.loadAttribute("sum_yt_minus_one_", sum_yt_minus_one_);
  adv.loadAttribute("sum_y_t_", sum_yt_);
  adv.loadAttribute("drift_", drift_);
  adv.loadAttribute("sigmaDrift_", sigmaDrift_);
  adv.loadAttribute("sum_t_yt_minus_one_", sum_t_yt_minus_one_);
  adv.loadAttribute("sum_t_yt_", sum_t_yt_);
  adv.loadAttribute("sum_ti_", sum_ti_);
  adv.loadAttribute("sum_ti_ti_", sum_ti_ti_);
  adv.loadAttribute("trend_", trend_);
  adv.loadAttribute("sigmaTrend_", sigmaTrend_);
  adv.loadAttribute("isComputedTrendCharacteristics_", isComputedTrendCharacteristics_);
}

END_NAMESPACE_OPENTURNS

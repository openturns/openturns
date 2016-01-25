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
#ifndef OPENTURNS_DICKEYFULLERTEST_HXX
#define OPENTURNS_DICKEYFULLERTEST_HXX

#include "PersistentObject.hxx"
#include "TestResult.hxx"
#include "Indices.hxx"
#include "TimeSeries.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class DickeyFullerTest
 *
 */

class OT_API DickeyFullerTest
  : public PersistentObject
{
  CLASSNAME;
public:

  /** Default constructor */
  DickeyFullerTest();

  /** Parameters constructor */
  explicit DickeyFullerTest(const TimeSeries & series,
                            const Bool verbose = true);

  /** Virtual constructor */
  virtual DickeyFullerTest * clone() const;

  /** Test H0: there is a unit root in a drift and linear trend model */
  TestResult testUnitRootInDriftAndLinearTrendModel(const NumericalScalar level = 0.95);

  /** Test H0: there is a unit root in a drift model */
  TestResult testUnitRootInDriftModel(const NumericalScalar level = 0.95);

  /** Test H0: there is a unit root in an AR1 model */
  TestResult testUnitRootInAR1Model(const NumericalScalar level = 0.95);

  /** Strategy of tests */
  TestResult runStrategy(const NumericalScalar level = 0.95);

  /** Test H0: there is no linear trend in a drift and linear trend model knowing that there is a unit root */
  TestResult testUnitRootAndNoLinearTrendInDriftAndLinearTrendModel(const NumericalScalar level = 0.95);

  /** Test H0: there is no linear trend in a drift and linear trend model knowing that there is no unit root */
  TestResult testNoUnitRootAndNoLinearTrendInDriftAndLinearTrendModel(const NumericalScalar level = 0.95);

  /** Test H0: there is no drift in a drift model knowing that there is a unit root */
  TestResult testUnitRootAndNoDriftInDriftModel(const NumericalScalar level = 0.95);

  /** Test the nullity of drift coefficient with assumption that there is no unit root */
  TestResult testNoUnitRootAndNoDriftInDriftModel(const NumericalScalar level = 0.95);

  /** Verbosity accessor */
  void setVerbose(const Bool verbose);
  Bool getVerbose() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:

  /** Test of general model ==> run a part of strategy */
  TestResult runTrendModelTest(const NumericalScalar level = 0.95);

  /** Test of drift model ==> run a part of strategy */
  TestResult runDriftModelStrategyTest(const NumericalScalar level = 0.95);

  /** Test of random walk model ==> run a part of strategy */
  TestResult runRandomWalkModelStrategyTest(const NumericalScalar level = 0.95);

  /** Estimate the coefficients of a model with drift and linear trend */
  void estimateDriftAndLinearTrendModel();

  /** Estimate the coefficients of a model with drift */
  void estimateDriftModel();

  /** Estimate the coefficients of an AR1 model */
  void estimateAR1Model();

private:

  /** Time series on which computation is done */
  TimeSeries timeSeries_;

  /** Size of underlying part */
  UnsignedInteger T_;

  /** Boolean verbose */
  Bool verbose_;

  /** Last model evaluated. Values are 1 (AR) to 3 (trend model) */
  UnsignedInteger lastModel_;

  /** rho value and the variance associated to the estimator */
  NumericalScalar rho_;
  NumericalScalar sigmaRho_;

  /** drift value  associated to the estimator */
  NumericalScalar drift_;
  NumericalScalar sigmaDrift_;

  /** trend value  associated to the estimator */
  NumericalScalar trend_;
  NumericalScalar sigmaTrend_;

  /** Boolean - Are the characteristic for model without drift computed? */
  Bool isComputedNoConstantCharacteristics_;

  /** Sum of 1/T * y_{t}*y_{t-1}, t=1,,,N  */
  NumericalScalar sum_yt_yt_minus_one_;

  /** Sum of 1/T * y_{t-1}*y_{t-1}, t=1,,,N  */
  NumericalScalar sum_squared_yt_minus_one_;

  /** Perform the computation of the previous quantities */
  void computeNoConstantCharacteristics();

  /** Boolean - Are the characteristic for model with drift computed? */
  Bool isComputedDriftCharacteristics_;

  /** Sum of 1/T * y_{t-1}, t=1,,,N  */
  NumericalScalar sum_yt_minus_one_;

  /** Sum of 1/T * y_{t}, t=1,,,N  */
  NumericalScalar sum_yt_;

  /** Perform the computation of the previous quantities */
  void computeDriftCharacteristics();

  /** Boolean - Are the characteristic for model with trend computed? */
  Bool isComputedTrendCharacteristics_;

  /** Sum of 1/T * y_{t-1}* t, t=1,,,N  */
  NumericalScalar sum_t_yt_minus_one_;

  /** Sum of 1/T * y_{t-1}* t, t=1,,,N  */
  NumericalScalar sum_t_yt_;

  /** Sum of 1/T * t_i, t=1,,,N  */
  NumericalScalar sum_ti_;

  /** Sum of 1/T * t_i * t_i, t=1,,,N  */
  NumericalScalar sum_ti_ti_;

  /** Perform the computation of the previous quantity */
  void computeTrendCharacteristics() ;

}; /* class DickeyFullerTest */

END_NAMESPACE_OPENTURNS
#endif /* OPENTURNS_DICKEYFULLERTEST_HXX */

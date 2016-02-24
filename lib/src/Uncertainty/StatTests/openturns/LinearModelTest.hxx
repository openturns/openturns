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
#ifndef OPENTURNS_LINEARMODELTEST_HXX
#define OPENTURNS_LINEARMODELTEST_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/TestResult.hxx"
#include "openturns/NumericalSample.hxx"
#include "openturns/LinearModel.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class LinearModelTest
 *
 */

class OT_API LinearModelTest
{
public:

  /**  */
  static TestResult LinearModelAdjustedRSquared(const NumericalSample & firstSample,
      const NumericalSample & secondSample,
      const LinearModel & linearModel,
      const NumericalScalar level = 0.95);

  /**  */
  static TestResult LinearModelAdjustedRSquared(const NumericalSample & firstSample,
      const NumericalSample & secondSample,
      const NumericalScalar level = 0.95);

  /**  */
  static TestResult LinearModelFisher(const NumericalSample & firstSample,
                                      const NumericalSample & secondSample,
                                      const LinearModel & linearModel,
                                      const NumericalScalar level = 0.95);

  /**  */
  static TestResult LinearModelFisher(const NumericalSample & firstSample,
                                      const NumericalSample & secondSample,
                                      const NumericalScalar level = 0.95);

  /**  */
  static TestResult LinearModelResidualMean(const NumericalSample & firstSample,
      const NumericalSample & secondSample,
      const LinearModel & linearModel,
      const NumericalScalar level = 0.95);

  /**  */
  static TestResult LinearModelResidualMean(const NumericalSample & firstSample,
      const NumericalSample & secondSample,
      const NumericalScalar level = 0.95);

  /**  */
  static TestResult LinearModelRSquared(const NumericalSample & firstSample,
                                        const NumericalSample & secondSample,
                                        const LinearModel & linearModel,
                                        const NumericalScalar level = 0.95);

  /**  */
  static TestResult LinearModelRSquared(const NumericalSample & firstSample,
                                        const NumericalSample & secondSample,
                                        const NumericalScalar level = 0.95);

protected:
  /** Generic invocation of a R script for testing a distribution against a sample */
  static TestResult RunTwoSamplesALinearModelRTest(const NumericalSample & firstSample,
      const NumericalSample & secondSample,
      const LinearModel & linearModel,
      const NumericalScalar level,
      const String & testName);
  LinearModelTest();
}; /* class LinearModelTest */

END_NAMESPACE_OPENTURNS
#endif /* OPENTURNS_LINEARMODELTEST_HXX */

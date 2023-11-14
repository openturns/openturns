//                                               -*- C++ -*-
/**
 *  @brief The test file of class DickeyFullerTest
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  const UnsignedInteger size = 100;

  ARMACoefficients ARCoefficients(Point(1, 0.3));
  ARMACoefficients MACoefficients(0);
  RegularGrid timeGrid(0.0, 0.1, size);
  // White noise ==> gaussian
  Normal myDistribution;
  WhiteNoise whiteNoise (myDistribution, timeGrid);
  ARMA myARMA(ARCoefficients, MACoefficients, whiteNoise);

  // A realization of the ARMA process
  // The realization is supposed to be of a stationnary process
  TimeSeries realization (myARMA.getRealization());

  // In the strategy of tests, one has to detect a trend tendency
  // We check if the time series writes as x_t = a +b * t + c * x_{t-1}
  // H0 = c is equal to one and thus
  // p-value threshold : probability of the H0 reject zone : 0.05
  // p-value : probability (test variable decision > test variable decision (statistic) evaluated on data)
  // Test = True <=> p-value > p-value threshold

  // Tests are run starting from general model to simplest model
  DickeyFullerTest myTestClass(realization);
  fullprint << "Drift and linear trend model=" << myTestClass.testUnitRootInDriftAndLinearTrendModel(0.05) << std::endl;
  fullprint << "Drift model=" << myTestClass.testUnitRootInDriftModel(0.05) << std::endl;
  fullprint << "AR1 model=" << myTestClass.testUnitRootInAR1Model(0.05) << std::endl;

  return ExitCode::Success;
}

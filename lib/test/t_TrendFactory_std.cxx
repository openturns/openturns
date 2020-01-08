//                                               -*- C++ -*-
/**
 *  @brief The test file of class TrendFactory for standard methods
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
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {

    Description inVar(1);
    inVar[0] = "t";
    Description outVar(1);
    outVar[0] = "y";
    Description formula(1);

    Collection<Function> functions(3);

    formula[0] = "1";
    functions[0] = SymbolicFunction(inVar, formula);

    formula[0] = "cos(2 * t)";
    functions[1] = SymbolicFunction(inVar, formula);

    formula[0] = "sin(2 * t)";
    functions[2] = SymbolicFunction(inVar, formula);


    // We build the weights
    Sample coefficients(0, 2);
    Point p(2);
    p[0] = 1.5;
    p[1] = 2.5;
    coefficients.add(p);
    p[0] = -0.5;
    p[1] = 0.5;
    coefficients.add(p);

    p[0] = 1.;
    p[1] = 1.;
    coefficients.add(p);

    // Third, build the function
    DualLinearCombinationFunction myFunction(functions, coefficients);


    // Fourth : we build a time series for estimation
    // it issued from a white noise
    const UnsignedInteger dimension = 2;

    // Fix the realization as a Normal
    Normal noiseDistribution(Point(dimension, 0.), Point(dimension, 1.), CorrelationMatrix(dimension));

    // TimeGrid parameters
    const UnsignedInteger N = 1000;
    const Scalar timeStart = 0.;
    const Scalar timeStep = 0.1;
    RegularGrid timeGrid(timeStart, timeStep, N);

    // White noise
    const WhiteNoise myWhiteNoise(noiseDistribution, timeGrid);

    TimeSeries realization(myWhiteNoise.getRealization());
    fullprint << "White noise realization = " << realization << std::endl;

    // We make a trend transform to the time series
    // We get a time series which contains values of time
    TrendTransform myTransformFunction(myFunction, timeGrid);
    TimeSeries myTimeSeries(timeGrid, myTransformFunction(realization.getValues()));
    fullprint << "myTimeSeries = " << myTimeSeries << std::endl;

    // We wants to get the coefficients using a factory
    // Build a factory using default constructor
    TrendFactory myDefaultFactory;
    fullprint << "myDefaultFactory = " << myDefaultFactory << std::endl;

    TrendTransform myEstimateTrend(myDefaultFactory.build(myTimeSeries, functions));
    fullprint << "myEstimateTrend = " << myEstimateTrend << std::endl;

    // We fix a new fitting algorithm
    myDefaultFactory.setFittingAlgorithm(KFold());
    fullprint << "myDefaultFactory = " << myDefaultFactory << std::endl;
    TrendTransform myNewEstimateTrend(myDefaultFactory.build(myTimeSeries, functions));
    fullprint << "myNewEstimateTrend = " << myNewEstimateTrend << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}

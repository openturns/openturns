//                                               -*- C++ -*-
/**
 *  @brief The test file of class NumericalMathFunction for standard methods
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
#include "OT.hxx"
#include "OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {

    // Test basic functionnalities
    //checkClassWithClassName<ComputedNumericalMathEvaluationImplementation>();

    /** Instance creation */
    NumericalMathFunction deviation("poutre");

    NumericalPoint inPoint1(4);
    inPoint1[0] = 210.e9;
    inPoint1[1] = 1000;
    inPoint1[2] = 1.4;
    inPoint1[3] = 2.e-6;

    NumericalPoint inPoint2(4);
    inPoint2[0] = 210.e9;
    inPoint2[1] = 1000;
    inPoint2[2] = 1.5;
    inPoint2[3] = 2.e-6;

    NumericalPoint inPoint3(4);
    inPoint3[0] = 210.e9;
    inPoint3[1] = 1000;
    inPoint3[2] = 1.6;
    inPoint3[3] = 2.e-6;


    NumericalPoint outPoint = deviation( inPoint2 );
    fullprint << "deviation = " << outPoint << std::endl;

    // Test the cache behavior
    deviation.enableCache();
    fullprint << "calls = " << deviation.getEvaluationCallsNumber() << " hits = " << deviation.getCacheHits() << std::endl;
    outPoint = deviation( inPoint2 );
    fullprint << "deviation = " << outPoint << std::endl;
    fullprint << "calls = " << deviation.getEvaluationCallsNumber() << " hits = " << deviation.getCacheHits() << std::endl;
    outPoint = deviation( inPoint2 );
    fullprint << "deviation = " << outPoint << std::endl;
    fullprint << "calls = " << deviation.getEvaluationCallsNumber() << " hits = " << deviation.getCacheHits() << std::endl;

    // Test samples
    NumericalSample inSample(3, deviation.getInputDimension());
    inSample[0] = inPoint1;
    inSample[1] = inPoint2;
    inSample[2] = inPoint3;

    NumericalSample outSample = deviation( inSample );
    fullprint << "deviation sample = " << outSample << std::endl;

    // Test timeseries
    TimeSeries inTimeSeries(3, deviation.getInputDimension());
    inTimeSeries.setValueAtIndex( 0, inPoint1 );
    inTimeSeries.setValueAtIndex( 1, inPoint2 );
    inTimeSeries.setValueAtIndex( 2, inPoint3 );

    TimeSeries outTimeSeries = deviation( inTimeSeries );
    fullprint << "deviation time series = " << outTimeSeries << std::endl;

    // Test gradient method
    Matrix gradient = deviation.gradient( inPoint2 );
    fullprint << "deviation.gradient = " << gradient << std::endl;

    // Test Hessian method
    SymmetricTensor tensor = deviation.hessian( inPoint2 );
    fullprint << "deviation.hessian = " << tensor << std::endl;

    // Test wrapper error
    NumericalPoint inPoint(4);
    inPoint[0] = 210.e9; // E
    inPoint[1] = 1000;   // F
    inPoint[2] = 1.5;    // L
    inPoint[3] = 0.   ;  // I is zero so an error should be raised

    try
    {
      NumericalPoint outPoint = deviation( inPoint );

      // We should never go here
      throw TestFailed( "ERROR: test should have failed. NumericalMathFunction did not failed as expected." );

    }
    catch (InternalException & ex)
    {
      fullprint << "exception ok" << std::endl;
      std::cerr  << "Got exception: " << ex << std::endl;
    }


    try
    {
      NumericalMathFunction deviation("unknown");

      // We should never go here
      throw TestFailed( "ERROR: test should have failed. NumericalMathFunction did not failed as expected." );

    }
    catch (Exception & ex)
    {
      fullprint << "exception ok" << std::endl;
      std::cerr << "Got exception: " << ex << std::endl;
    }

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}

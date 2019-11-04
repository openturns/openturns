//                                               -*- C++ -*-
/**
 *  @brief The test file of class NearestPointChecker for standard methods
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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

  try
  {

    // Function
    UnsignedInteger sampleSize = 20;
    Description input(4);
    input[0] = "x1";
    input[1] = "x2";
    input[2] = "x3";
    input[3] = "x4";
    SymbolicFunction levelFunction(input, Description(1, "x1+2*x2-3*x3+4*x4"));
    LessOrEqual myOperator;
    Scalar threshold = 2.0;
    Sample mySample(0, levelFunction.getInputDimension());
    Scalar random = 0.1;
    for(UnsignedInteger index = 0; index < sampleSize; index++)
    {
      Point point(levelFunction.getInputDimension());
      Scalar norm = 0.0;
      for(UnsignedInteger coordinate = 0; coordinate < levelFunction.getInputDimension(); coordinate++)
      {
        point[coordinate] = sqrt(-2.0 * log(random));
        random = fmod(random + sqrt(2.0), 1.0);
        point[coordinate] *= cos(2.0 * atan(1.0) * random);
        norm += point[coordinate] * point[coordinate];
      }
      for(UnsignedInteger coordinate = 0; coordinate < levelFunction.getInputDimension(); coordinate++)
      {
        point[coordinate] /= sqrt(norm);
      }
      mySample.add(point);
    }
    NearestPointChecker myNearestPointChecker(levelFunction, myOperator, threshold, mySample);
    for(UnsignedInteger index = 0; index < sampleSize; index++)
    {
      fullprint << mySample[index] << std::endl;
    }
    myNearestPointChecker.run();
    fullprint << "myNearestPointChecker = " << myNearestPointChecker << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}

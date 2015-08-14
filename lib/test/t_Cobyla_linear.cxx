//                                               -*- C++ -*-
/**
 *  @brief The test file of class Cobyla for solving a linear problem
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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

String printNumericalPoint(const NumericalPoint & point, const UnsignedInteger digits)
{
  OSS oss;
  oss << "[";
  NumericalScalar eps(pow(0.1, 1.0 * digits));
  for (UnsignedInteger i = 0; i < point.getDimension(); i++)
  {
    oss << std::fixed << std::setprecision(digits) << (i == 0 ? "" : ",") << Bulk<double>((fabs(point[i]) < eps) ? fabs(point[i]) : point[i]);
  }
  oss << "]";
  return oss;
}

int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {

    try
    {
      // Test function operator ()
      Description input(4);
      input[0] = "x1";
      input[1] = "x2";
      input[2] = "x3";
      input[3] = "x4";
      NumericalMathFunction levelFunction(input, Description(1, "y1"), Description(1, "x1+2*x2-3*x3+4*x4"));
      CobylaSpecificParameters specific;
      NumericalPoint startingPoint(4, 0.0);
      Cobyla myAlgorithm(specific, levelFunction);
      myAlgorithm.setStartingPoint(startingPoint);
      myAlgorithm.setLevelValue(3.0);
      fullprint << "myAlgorithm = " << myAlgorithm << std::endl;
      myAlgorithm.run();
      fullprint << "result = " << printNumericalPoint(myAlgorithm.getResult().getMinimizer(), 4) << std::endl;
    }
    catch (NoWrapperFileFoundException & ex)
    {
      throw TestFailed(ex.__repr__());
    }


  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}

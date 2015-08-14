//                                               -*- C++ -*-
/**
 *  @brief The test file of class Tnc for standard methods
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

int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {
    Description input(4);
    input[0] = "x1";
    input[1] = "x2";
    input[2] = "x3";
    input[3] = "x4";
    NumericalMathFunction levelFunction(input, Description(1, "y1"), Description(1, "x1+2*x2-3*x3+4*x4"));
    TNCSpecificParameters specific;
    NumericalPoint startingPoint(4, 1.0);
    Interval bounds(NumericalPoint(4, -3.0), NumericalPoint(4, 5.0));
    TNC myAlgorithm(specific, levelFunction, bounds, startingPoint, TNC::Result::MINIMIZATION);
    myAlgorithm.setMaximumEvaluationsNumber(100);
    myAlgorithm.setMaximumAbsoluteError(1.0e-10);
    myAlgorithm.setMaximumRelativeError(1.0e-10);
    myAlgorithm.setMaximumObjectiveError(1.0e-10);
    myAlgorithm.setMaximumConstraintError(1.0e-10);
    fullprint << "myAlgorithm = " << myAlgorithm << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}

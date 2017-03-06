//                                               -*- C++ -*-
/**
 * @brief The test file of class SymbolicGradient for standard methods
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {
    Description inputNames(3);
    inputNames[0] = "x0";
    inputNames[1] = "x1";
    inputNames[2] = "x2";
    Description outputNames(1);
    outputNames[0] = "y0";
    Description formulas(1);
    formulas[0] = "x0^2+2*x1+3*x2^3";
    SymbolicEvaluation evaluation(inputNames, outputNames, formulas);
    SymbolicGradient gradient(evaluation);
    fullprint << "gradient=" << gradient << std::endl;
    NumericalPoint point(3);
    point[0] = -1.0;
    point[0] =  4.0;
    point[1] = -4.0;
    fullprint << "value at " << point << "=" << gradient.gradient(point) << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}

//                                               -*- C++ -*-
/**
 *  @brief The test file of class LinearCombinationGradient for standard methods
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
    // First, build two functions from R^3->R
    Description inVar(3);
    inVar[0] = "x1";
    inVar[1] = "x2";
    inVar[2] = "x3";
    Description outVar(1);
    outVar[0] = "y";
    Description formula(1);
    formula[0] = "x1^3 * sin(x2 + 2.5 * x3) - (x1 + x2)^2 / (1.0 + x3^2)";
    LinearCombinationEvaluation::FunctionCollection functions(2);
    functions[0] = SymbolicFunction(inVar, formula);
    formula[0] = "exp(-x1 * x2 + x3) / cos(1.0 + x2 * x3 - x1)";
    functions[1] = SymbolicFunction(inVar, formula);
    // Second, build the weights
    Point coefficients(2);
    coefficients[0] = 0.3;
    coefficients[1] = 2.9;
    LinearCombinationGradient myGradient(LinearCombinationEvaluation(functions, coefficients));
    Point inPoint(3);
    inPoint[0] = 1.2;
    inPoint[1] = 2.3;
    inPoint[2] = 3.4;
    fullprint << "myGradient=" << myGradient << std::endl;
    fullprint << "Value at " << inPoint << "=" << myGradient.gradient(inPoint) << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}

//                                               -*- C++ -*-
/**
 *  @brief The test file of class Function for linear combinations
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
    // First, build two functions from R^3->R^3
    Description inVar(2);
    inVar[0] = "x1";
    inVar[1] = "x2";
    Description formula(3);
    formula[0] = "x1^3 * sin(x2 + 2.5 * x1) - (x1 + x2)^2 / (1.0 + x2^2)";
    formula[1] = "x2^3 * sin(x2 + 2.5 * x1) - (x2 + x1)^2 / (1.0 + x1^2)";
    formula[2] = "x1^3 * sin(x1 + 2.5 * x2) - (x2 + x1)^2 / (1.0 + x2^2)";
    LinearCombinationEvaluation::FunctionCollection functions(2);
    functions[0] = SymbolicFunction(inVar, formula);
    formula[0] = "exp(-x1 * x2 + x1) / cos(1.0 + x2 * x2 - x1)";
    formula[0] = "exp(-x2 * x1 + x1) / cos(1.0 + x2 * x1 - x2)";
    formula[0] = "exp(-x1 * x1 + x2) / cos(1.0 + x1 * x2 - x2)";
    functions[1] = SymbolicFunction(inVar, formula);
    // Second, build the weights
    Point coefficients(2);
    coefficients[0] = 0.3;
    coefficients[1] = 2.9;
    // Third, build the function
    LinearCombinationFunction myFunction(functions, coefficients);
    Point inPoint(2);
    inPoint[0] = 1.2;
    inPoint[1] = 2.3;
    std::cout << "myFunction=" << myFunction << std::endl;
    std::cout << "Value at " << inPoint << "=" << myFunction(inPoint) << std::endl;
    std::cout << "Gradient at " << inPoint << "=" << myFunction.gradient(inPoint) << std::endl;
    std::cout << "Hessian at " << inPoint << "=" << myFunction.hessian(inPoint) << std::endl;
    for (UnsignedInteger i = 0; i < myFunction.getOutputDimension(); ++i)
    {
      std::cout << "Marginal " << i << "=" << myFunction.getMarginal(i) << std::endl;
    }
    Indices indices(2);
    indices[0] = 0;
    indices[1] = 1;
    std::cout << "Marginal (0,1)=" << myFunction.getMarginal(indices) << std::endl;
    indices[0] = 0;
    indices[1] = 2;
    std::cout << "Marginal (0,2)=" << myFunction.getMarginal(indices) << std::endl;
    indices[0] = 1;
    indices[1] = 2;
    std::cout << "Marginal (1,2)=" << myFunction.getMarginal(indices) << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}

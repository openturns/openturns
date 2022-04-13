//                                               -*- C++ -*-
/**
 *  @brief The test file of class Function for dual_linear combinations
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
    Description formula(1);
    formula[0] = "x1^3 * sin(x2 + 2.5 * x3) - (x1 + x2)^2 / (1.0 + x3^2)";
    DualLinearCombinationEvaluation::FunctionCollection functions(2);
    functions[0] = SymbolicFunction(inVar, formula);
    formula[0] = "exp(-x1 * x2 + x3) / cos(1.0 + x2 * x3 - x1)";
    functions[1] = SymbolicFunction(inVar, formula);
    // Second, build the weights
    Sample coefficients(0, 3);
    Point p(3);
    p[0] = 1.5;
    p[1] = 2.5;
    p[2] = -0.5;
    coefficients.add(p);
    p[0] = -3.5;
    p[1] = 0.5;
    p[2] = -1.5;
    coefficients.add(p);
    // Third, build the function
    DualLinearCombinationFunction myFunction(functions, coefficients);
    Point inPoint(3);
    inPoint[0] = 1.2;
    inPoint[1] = 2.3;
    inPoint[2] = 3.4;
    fullprint << "myFunction=" << myFunction << std::endl;
    fullprint << "Value at " << inPoint << "=" << myFunction(inPoint) << std::endl;
    fullprint << "Gradient at " << inPoint << "=" << myFunction.gradient(inPoint) << std::endl;
    PlatformInfo::SetNumericalPrecision(5);
    fullprint << "Hessian at " << inPoint << "=" << myFunction.hessian(inPoint) << std::endl;
    for (UnsignedInteger i = 0; i < myFunction.getOutputDimension(); ++i)
    {
      fullprint << "Marginal " << i << "=" << myFunction.getMarginal(i) << std::endl;
    }
    Indices indices(2);
    indices[0] = 0;
    indices[1] = 1;
    fullprint << "Marginal (0,1)=" << myFunction.getMarginal(indices) << std::endl;
    indices[0] = 0;
    indices[1] = 2;
    fullprint << "Marginal (0,2)=" << myFunction.getMarginal(indices) << std::endl;
    indices[0] = 1;
    indices[1] = 2;
    fullprint << "Marginal (1,2)=" << myFunction.getMarginal(indices) << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}

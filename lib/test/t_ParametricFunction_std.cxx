//                                               -*- C++ -*-
/**
 *  @brief The test file of class NumericalMathFunction for parametric functions
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
    Description inVar(Description::BuildDefault(4, "x"));
    Description formulas(3);
    formulas[0] = "sin(x0) + x1 * cos(x2) / exp(x3)";
    formulas[1] = "-2.0 * x0 + x1 * x2^2 + cos(x3)";
    formulas[2] = "x0 / (abs(x1) * x2^2 + x3 + 1.0)";
    SymbolicFunction f(inVar, formulas);
    Indices set(2);
    set[0] = 3;
    set[1] = 1;
    Bool parametersSet = true;
    NumericalPoint x(set.getSize());
    x[0] = 1.0;
    x[1] = 2.0;
    NumericalPoint referencePoint(2, 0.85);
    ParametricFunction g2(f, set, referencePoint, parametersSet);
    g2.enableHistory();
    fullprint << "g2=" << g2 << std::endl;
    fullprint << "g2(x)=" << g2(x) << std::endl;

    // point / parameter history
    fullprint << "point history=" << g2.getInputPointHistory() << std::endl;
    fullprint << "parameter history=" << g2.getInputParameterHistory() << std::endl;

    // marginal extraction
    NumericalMathFunction g2_0(g2.getMarginal(0));
    NumericalPoint theta(2, 0.2);
    theta[1] = 50.0;
    fullprint << "g2_0(x, theta)=" << g2_0(x, theta) << std::endl;

    // gradient / parameters
    fullprint << "dg2_0/dtheta(x)=" << g2_0.parameterGradient(x) << std::endl;

    // single point, several parameters
    NumericalSample thetas(3, 2);
    thetas[0] = theta;
    thetas[1] = NumericalPoint(2, 0.3);
    thetas[1] = NumericalPoint(2, 0.4);
    fullprint << "g2_0(x, thetas)=" << g2_0(x, thetas) << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}

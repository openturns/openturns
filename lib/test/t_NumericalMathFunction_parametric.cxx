//                                               -*- C++ -*-
/**
 *  @brief The test file of class NumericalMathFunction for parametric functions
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
    Description outVar(Description::BuildDefault(3, "y"));
    Description formulas(3);
    formulas[0] = "sin(x0) + x1 * cos(x2) / exp(x3)";
    formulas[1] = "-2.0 * x0 + x1 * x2^2 + cos(x3)";
    formulas[2] = "x0 / (abs(x1) * x2^2 + x3 + 1.0)";
    NumericalMathFunction f(inVar, outVar, formulas);
    Indices set(2);
    set[0] = 3;
    set[1] = 1;
    Bool parametersSet = true;
    NumericalMathFunction g(f, set, parametersSet);
    fullprint << "g=" << g << std::endl;
    NumericalPoint x(set.getSize());
    x[0] = 1.0;
    x[1] = 2.0;
    fullprint << "g(x)=" << g(x) << std::endl;
    NumericalPoint referencePoint(4, 0.85);
    // with reference point
    NumericalMathFunction g2(f, set, referencePoint, parametersSet);
    fullprint << "g2=" << g2 << std::endl;
    fullprint << "g2(x)=" << g2(x) << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}

//                                               -*- C++ -*-
/**
 *  @brief The test file of class NumericalMathFunction for drawing methods
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
    // Simplified interfaces
    // First, try 1D drawing
    {
      NumericalMathFunction f("x", "sin(2*_pi*x)*exp(-x^2/2)", "y");
      fullprint << "f=" << f << std::endl;
      Graph graph(f.draw(-1.2, 1.2, 32));
      fullprint << "graph=" << graph << std::endl;
      graph.draw("NMFDraw1D");
    }
    // Second, try 2D drawing
    {
      Description inputVar(2);
      inputVar[0] = "x";
      inputVar[1] = "y";
      Description formula(1);
      formula[0] = "2.0+x-2*y+x*y-x^2-3*y^2+x*y^2";
      Description outputVar(1, "z");
      NumericalMathFunction f(inputVar, outputVar, formula);
      fullprint << "f=" << f << std::endl;
      Graph graph(f.draw(NumericalPoint(2, -10.0), NumericalPoint(2, 10.0), Indices(2, 21)));
      fullprint << "graph=" << graph << std::endl;
      graph.draw("NMFDraw2D");
    }
    // Full interfaces
    {
      Description inputVars(3);
      inputVars[0] = "x0";
      inputVars[1] = "x1";
      inputVars[2] = "x2";
      Description formulas(2);
      formulas[0] = "x0 * sin(x1 + 2.0 * x2) - 2.0 * x1 * cos(3.0 * x0 - x2)";
      formulas[1] = "x1 * cos(x2 + 2.0 * x1) + 2.0 * x0 * cos(3.0 * x1 - x0)";
      Description outputVars(2);
      outputVars[0] = "y0";
      outputVars[1] = "y1";
      NumericalMathFunction f(inputVars, outputVars, formulas);
      NumericalPoint centralPoint(3);
      centralPoint[0] = 1.0;
      centralPoint[1] = -0.5;
      centralPoint[2] = 1.5;
      // First output as a function of first input around central point
      Graph graph1D(f.draw(0, 0, centralPoint, -5.0, 5.0, 32));
      fullprint << "graph1D=" << graph1D << std::endl;
      graph1D.draw("NMFDraw1DFull");
      // Second output as a function of second and third inputs around central point
      Graph graph2D(f.draw(1, 2, 1, centralPoint, NumericalPoint(2, -5.0), NumericalPoint(2, 5.0), Indices(2, 21)));
      fullprint << "graph1D=" << graph2D << std::endl;
      graph2D.draw("NMFDraw2DFull");
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}

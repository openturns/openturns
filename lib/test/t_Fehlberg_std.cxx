//                                               -*- C++ -*-
/**
 *  @brief The test file of class Secant for standard methods
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
#include <cmath>

using namespace OT;
using namespace OT::Test;

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {
    Description inputVariables(3);
    inputVariables[0] = "t";
    inputVariables[1] = "y0";
    inputVariables[2] = "y1";
    Description formulas(2);
    formulas[0] = "t - y0";
    formulas[1] = "y1 + t^2";
    SymbolicFunction f(inputVariables, formulas);
    Point initialState(2);
    initialState[0] =  1.0;
    initialState[1] = -1.0;
    UnsignedInteger nt = 100;
    Point timeGrid(nt);
    for (UnsignedInteger i = 0; i < nt; ++i)
    {
      timeGrid[i] = pow(i, 2.0) / pow(nt - 1.0, 2.0);
    }
    ParametricFunction phi(f, Indices(1, 0), Point(1, 0.0));
    fullprint << "time grid=" << timeGrid << std::endl;
    Point ref(2);
    Scalar t = timeGrid[nt - 1];
    ref[0] = -1.0 + t + 2.0 * exp(-t);
    ref[1] = -2.0 + -2.0 * t - t * t + exp(t);
    {
      Fehlberg solver(phi, 1.0e-4, 1);
      fullprint << "ODE solver=" << solver << std::endl;
      Sample result(solver.solve(initialState, timeGrid));
      fullprint << "result=" << result << std::endl;
      fullprint << "last value=" << std::setprecision(16) << result[nt - 1] << std::endl;
      fullprint << "ref. value=" << std::setprecision(16) << ref << std::endl;
    }
    {
      Fehlberg solver(phi, 1.0e-6, 2);
      fullprint << "ODE solver=" << solver << std::endl;
      Sample result(solver.solve(initialState, timeGrid));
      fullprint << "result=" << result << std::endl;
      fullprint << "last value=" << std::setprecision(16) << result[nt - 1] << std::endl;
      fullprint << "ref. value=" << std::setprecision(16) << ref << std::endl;
    }
    {
      Fehlberg solver(phi, 1.0e-10, 4);
      fullprint << "ODE solver=" << solver << std::endl;
      Sample result(solver.solve(initialState, timeGrid));
      fullprint << "result=" << result << std::endl;
      fullprint << "last value=" << std::setprecision(16) << result[nt - 1] << std::endl;
      fullprint << "ref. value=" << std::setprecision(16) << ref << std::endl;
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}

//                                               -*- C++ -*-
/**
 *  @brief The test file of class LeastSquaresEquationsSolver for standard methods
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
    /** Analytical construction */
    Description input = {"x","y"};
    Description formulas = {"y*x-sin(2*x)","1 + cos(y) + x"};

    SymbolicFunction analytical(input, formulas);
    LeastSquaresEquationsSolver myAlgo;
    myAlgo.setResidualError(1e-5);
    fullprint << "myAlgo=" << myAlgo << std::endl;
    Point startingPoint = {2.0,1.0};
    OSS oss;
    oss.setPrecision(5);
    fullprint << "x=" << String(oss << myAlgo.solve(analytical, startingPoint)) << std::endl;
    fullprint << "myAlgo=" << myAlgo << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}

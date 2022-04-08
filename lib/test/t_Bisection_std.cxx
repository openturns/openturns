//                                               -*- C++ -*-
/**
 *  @brief The test file of class Bisection for standard methods
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
    /** Analytical construction */
    Description input(1);
    input[0] = "x";
    Description formulas(1);
    formulas[0] = "x-cos(2*x)";
    SymbolicFunction analytical(input, formulas);
    Bisection myAlgo;
    fullprint << "myAlgo=" << myAlgo << std::endl;
    Scalar value = 0.0;
    fullprint << "Solve " << formulas[0] << "=" << value << " for " << input[0] << std::endl;
    OSS oss;
    oss.setPrecision(5);
    fullprint << "x=" << String(oss << myAlgo.solve(analytical, value, 0.0, 3.0)) << std::endl;
    fullprint << "myAlgo=" << myAlgo << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}

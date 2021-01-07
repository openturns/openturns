//                                               -*- C++ -*-
/**
 *  @brief The test file of class Function for indicator
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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

    /* Analytical construction */
    Description input(2);
    input[0] = "x0";
    input[1] = "x1";
    Description formulas(1);
    formulas[0] = "x0+x1";
    SymbolicFunction analytical(input, formulas);

    fullprint << "function=" << analytical << std::endl;

    /* Create indicator function */
    IndicatorFunction indicator(analytical, Less(), 0.0);
    /* Does it work? */
    Point x(Point(2, 1.0));
    Scalar value = analytical(x)[0];
    fullprint << "Value of the function=" << value << " value of the indicator=" << indicator(x) << std::endl;
    x = Point(2, -1.0);
    value = analytical(x)[0];
    fullprint << "Value of the function=" << value << " value of the indicator=" << indicator(x) << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}

//                                               -*- C++ -*-
/**
 *  @brief The test file of class MonomialFunction for standard methods
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
    // Default constructor: null polynomial
    MonomialFunction P0;
    Scalar point = 1.2;
    fullprint <<  "P0 :" << P0.__str__() << std::endl;
    fullprint <<  "P0(" << point <<  ")=" << P0(point) << std::endl;

    // Standard constructor : set degree/coefficients
    for (UnsignedInteger degree = 1; degree < 6; ++ degree)
    {
      fullprint << std::endl;
      MonomialFunction P(degree);
      // Print P + P operators
      fullprint << "P :" <<  P.__str__() << std::endl;
      fullprint << "P(1) = " << P(1.0)
                << ", dP(1) = " << P.gradient(1.0)
                <<  ", d^2P(1) = " <<  P.hessian(1.0)  << std::endl;
      // Print Q + Q operators
    }

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}


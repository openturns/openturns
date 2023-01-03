//                                               -*- C++ -*-
/**
 *  @brief The test file of class ARMAState
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

    RandomGenerator::SetSeed(0);
    /* Constructor with size */
    const UnsignedInteger dim = 1;
    const UnsignedInteger p = 6;
    const UnsignedInteger q = 4;

    Sample valuesX(p, dim);
    Sample valuesEpsilon(q, dim);

    /**/
    for(UnsignedInteger j = 0 ; j < dim ; ++j)
    {
      // Fill the AR-part (the last p-coefficients X_{-1}, X{-2},..., X_{-p})
      for(UnsignedInteger i = 0 ; i < p ; ++i)
      {
        valuesX[i][j] = 2.0 * i + 3.0 * j + 1.0;
      }

      // Fill the MA-part (the last p-coefficients \epsilon_{-1}, \epsilon_{-2},..., \epsilon_{-p})
      for(UnsignedInteger i = 0 ; i < q ; ++i)
      {
        valuesEpsilon[i][j] = RandomGenerator::Generate() ;
      }

    }

    /** Print the initial state of the ARMA : coefficients*/
    fullprint << "X values = " << valuesX << std::endl;
    fullprint << "Epsilon values = " << valuesEpsilon << std::endl;

    // default constructor
    ARMAState myDefaultState;
    fullprint << "ARMAState with default constructor = " << myDefaultState.__repr__() << std::endl;

    // parameters constructor
    ARMAState myState(valuesX, valuesEpsilon);
    fullprint << "ARMAState  = " << myState.__repr__() << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}

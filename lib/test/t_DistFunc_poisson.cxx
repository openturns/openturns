//                                               -*- C++ -*-
/**
 *  @brief The test file of class DistFunc for standard methods
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
  setRandomGenerator();

  try
  {
    // Poisson related functions
    {
      // rPoisson
      NumericalScalar lambdaMin = 0.2;
      NumericalScalar lambdaMax = 5.0;
      UnsignedInteger n1 = 5;
      UnsignedInteger nR = 5;
      for (UnsignedInteger i1 = 0; i1 < n1; ++i1)
      {
        NumericalScalar lambda = lambdaMin + (lambdaMax - lambdaMin) * i1 / (n1 - 1);
        for (UnsignedInteger iR = 0; iR < nR; ++iR)
        {
          fullprint << "rPoisson(" << lambda << ")=" << DistFunc::rPoisson(lambda) << std::endl;
        }
      }
    } // rPoisson
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}

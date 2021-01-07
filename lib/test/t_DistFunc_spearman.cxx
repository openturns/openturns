//                                               -*- C++ -*-
/**
 *  @brief The test file of class DistFunc for standard methods
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
  setRandomGenerator();

  try
  {
    // Spearman related functions
    Point lSize(5);
    lSize[0] = 5;
    lSize[1] = 10;
    lSize[2] = 25;
    lSize[3] = 50;
    lSize[4] = 1000;
    for (UnsignedInteger iSize = 0; iSize < lSize.getSize(); ++iSize)
    {
      const UnsignedInteger size = lSize[iSize];
      for (UnsignedInteger iTies = 0; iTies < 2; ++iTies)
      {
        const Bool ties = iTies == 0;
        for (UnsignedInteger iTail = 0; iTail < 2; ++iTail)
        {
          const Bool tail = iTail == 0;
          for (int iRho = -10; iRho < 11; ++iRho)
          {
            const Scalar rho = 0.1 * iRho;
            fullprint << "size=" << size << ", ties=" << ties << ", tail=" << tail << ", rho=" << rho << ", p=" << DistFunc::pSpearmanCorrelation(size, rho, tail, ties) << std::endl;
          } // rho
        } // tail
      } // ties
    } // size
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}

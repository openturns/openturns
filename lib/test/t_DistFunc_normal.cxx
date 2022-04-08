//                                               -*- C++ -*-
/**
 *  @brief The test file of class DistFunc for standard methods
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
  setRandomGenerator();

  try
  {
    // Normal related functions
    {
      // pNormal
      Scalar xMin = 0.1;
      Scalar xMax = 0.9;
      UnsignedInteger nX = 10;
      Point grid(nX);
      for (UnsignedInteger iX = 0; iX < nX; ++iX)
      {
        Scalar x = xMin + (xMax - xMin) * iX / (nX - 1);
        grid[iX] = x;
        fullprint << "pNormal(" << x << ")=" << DistFunc::pNormal(x) << ", complementary=" << DistFunc::pNormal(x, true) << std::endl;
      }
      fullprint << "pNormal(" << grid << ")=" << DistFunc::pNormal(grid) << std::endl;
    } // pNormal
    {
      // qNormal
      Scalar qMin = 0.1;
      Scalar qMax = 0.9;
      UnsignedInteger nQ = 10;
      Point grid(nQ);
      for (UnsignedInteger iQ = 0; iQ < nQ; ++iQ)
      {
        Scalar q = qMin + (qMax - qMin) * iQ / (nQ - 1);
        grid[iQ] = q;
        fullprint << "qNormal(" << q << ")=" << DistFunc::qNormal(q) << ", complementary=" << DistFunc::qNormal(q, true) << std::endl;
      }
      fullprint << "qNormal(" << grid << ")=" << DistFunc::qNormal(grid) << std::endl;
    } // qNormal
    {
      // rNormal
      UnsignedInteger nR = 10;
      for (UnsignedInteger iR = 0; iR < nR; ++iR)
      {
        fullprint << "rNormal()=" << DistFunc::rNormal() << std::endl;
      }
    } // rNormal
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}

//                                               -*- C++ -*-
/**
 *  @brief The test file of class DistFunc for standard methods
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
    // Binomial related functions
    {
      // dBinomial
      Scalar pMin = 0.1;
      Scalar pMax = 0.9;
      UnsignedInteger nMin = 2;
      UnsignedInteger nMax = 12;
      UnsignedInteger n1 = 5;
      for (UnsignedInteger i1 = 0; i1 < n1; ++i1)
      {
        Scalar p = pMin + i1 * (pMax - pMin) / (n1 - 1.0);
        for (UnsignedInteger n = nMin; n <= nMax; ++n)
        {
          for (UnsignedInteger k = 0; k <= n; ++k)
          {
            fullprint << "dBinomial(" << n << ", " << p << ", " << k << ")=" << std::setprecision(3) << DistFunc::dBinomial(n, p, k) << std::endl;
          }
        }
      }
    } // dBinomial
    {
      // logdBinomial
      Scalar pMin = 0.1;
      Scalar pMax = 0.9;
      UnsignedInteger nMin = 2;
      UnsignedInteger nMax = 12;
      UnsignedInteger n1 = 5;
      for (UnsignedInteger i1 = 0; i1 < n1; ++i1)
      {
        Scalar p = pMin + i1 * (pMax - pMin) / (n1 - 1.0);
        for (UnsignedInteger n = nMin; n <= nMax; ++n)
        {
          for (UnsignedInteger k = 0; k <= n; ++k)
          {
            fullprint << "logdBinomial(" << n << ", " << p << ", " << k << ")=" << DistFunc::logdBinomial(n, p, k) << std::endl;
          }
        }
      }
    } // logdBinomial
    {
      // rBinomial
      Scalar pMin = 0.1;
      Scalar pMax = 0.9;
      UnsignedInteger nMin = 2;
      UnsignedInteger nMax = 12;
      UnsignedInteger n1 = 5;
      UnsignedInteger nR = 5;
      for (UnsignedInteger i1 = 0; i1 < n1; ++i1)
      {
        Scalar p = pMin + i1 * (pMax - pMin) / (n1 - 1.0);
        for (UnsignedInteger n = nMin; n <= nMax; ++n)
        {
          for (UnsignedInteger iR = 0; iR < nR; ++iR)
          {
            fullprint << "rBinomial()=" << DistFunc::rBinomial(n, p) << std::endl;
          }
        }
      }
    } // rBinomial
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}

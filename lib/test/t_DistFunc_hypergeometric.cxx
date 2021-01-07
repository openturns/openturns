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
    // Hypergeometric related functions
    {
      // dHypergeometric
      UnsignedInteger nMin = 2;
      UnsignedInteger nMax = 12;
      for (UnsignedInteger n = nMin; n <= nMax; ++n)
      {
        for (UnsignedInteger k = 1; k < n; ++k)
        {
          for (UnsignedInteger m = 1; m < n; ++m)
          {
            for (UnsignedInteger x = (k + m > n ? k + m - n : 0); x <= std::min(k, m); ++x)
            {
              fullprint << "dHypergeometric(" << n << ", " << k << ", " << m << ", " << x << ")=" << DistFunc::dHypergeometric(n, k, m, x) << std::endl;
            } // x
          } // m
        } // k
      } // n
    } // dHypergeometric
    {
      // logdHypergeometric
      UnsignedInteger nMin = 2;
      UnsignedInteger nMax = 12;
      for (UnsignedInteger n = nMin; n <= nMax; ++n)
      {
        for (UnsignedInteger k = 1; k < n; ++k)
        {
          for (UnsignedInteger m = 1; m < n; ++m)
          {
            for (UnsignedInteger x = (k + m > n ? k + m - n : 0); x <= std::min(k, m); ++x)
            {
              fullprint << "logdHypergeometric(" << n << ", " << k << ", " << m << ", " << x << ")=" << DistFunc::logdHypergeometric(n, k, m, x) << std::endl;
            } // x
          } // m
        } // k
      } // n
    } // logdHypergeometric
    {
      // pHypergeometric
      UnsignedInteger nMin = 2;
      UnsignedInteger nMax = 12;
      for (UnsignedInteger n = nMin; n <= nMax; ++n)
      {
        for (UnsignedInteger k = 1; k < n; ++k)
        {
          for (UnsignedInteger m = 1; m < n; ++m)
          {
            for (UnsignedInteger x = (k + m > n ? k + m - n : 0); x <= std::min(k, m); ++x)
            {
              fullprint << "pHypergeometric(" << n << ", " << k << ", " << m << ", " << x << ")=" << DistFunc::pHypergeometric(n, k, m, x) << std::endl;
            } // x
          } // m
        } // k
      } // n
    } // pHypergeometric
    {
      // rHypergeometric
      UnsignedInteger nMin = 2;
      UnsignedInteger nMax = 12;
      for (UnsignedInteger n = nMin; n <= nMax; ++n)
      {
        for (UnsignedInteger k = 1; k < n; ++k)
        {
          for (UnsignedInteger m = 1; m < n; ++m)
          {
            for (UnsignedInteger x = (k + m > n ? k + m - n : 0); x <= std::min(k, m); ++x)
            {
              fullprint << "rHypergeometric(" << n << ", " << k << ", " << m << ")=" << DistFunc::rHypergeometric(n, k, m) << std::endl;
            } // rng
          } // m
        } // k
      } // n
    } // rHypergeometric
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}

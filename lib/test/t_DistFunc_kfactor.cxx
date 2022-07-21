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
    // kFactor related functions
    {
      // kFactor
      UnsignedInteger nMin = 2;
      UnsignedInteger nMax = 3;
      Scalar pMin = 0.1;
      Scalar pMax = 0.9;
      UnsignedInteger nP = 2;
      Scalar alphaMin = 0.1;
      Scalar alphaMax = 0.9;
      UnsignedInteger nAlpha = 2;
      for (UnsignedInteger n = nMin; n <= nMax; ++n)
      {
        for (UnsignedInteger iP = 0; iP <= nP; ++iP)
        {
          Scalar p = pMin + ((pMax - pMin) * iP) / nP;
          for (UnsignedInteger iAlpha = 0; iAlpha <= nAlpha; ++iAlpha)
          {
            Scalar alpha = alphaMin + (alphaMax - alphaMin) * iAlpha / nAlpha;
            fullprint << "kFactor(" << 2 * n << ", " << p << ", " << alpha << ")=" << DistFunc::kFactor(2 * n, p, alpha) << std::endl;
          } // alpha
        } // p
      } // n
    } // kFactor
    {
      // kFactorPooled
      UnsignedInteger nMin = 2;
      UnsignedInteger nMax = 3;
      UnsignedInteger mMin = 1;
      UnsignedInteger mMax = 2;
      Scalar pMin = 0.1;
      Scalar pMax = 0.9;
      UnsignedInteger nP = 2;
      Scalar alphaMin = 0.1;
      Scalar alphaMax = 0.9;
      UnsignedInteger nAlpha = 2;
      for (UnsignedInteger n = nMin; n <= nMax; ++n)
      {
        for (UnsignedInteger m = mMin; m <= mMax; ++m)
        {
          for (UnsignedInteger iP = 0; iP <= nP; ++iP)
          {
            Scalar p = pMin + ((pMax - pMin) * iP) / nP;
            for (UnsignedInteger iAlpha = 0; iAlpha <= nAlpha; ++iAlpha)
            {
              Scalar alpha = alphaMin + (alphaMax - alphaMin) * iAlpha / nAlpha;
              fullprint << "kFactorPooled(" << 2 * n << ", " << m << ", " << p << ", " << alpha << ")=" << DistFunc::kFactorPooled(2 * n, m, p, alpha) << std::endl;
            } // alpha
          } // p
        } // m
      } // n
    } // kFactorPooled
  } // try
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}

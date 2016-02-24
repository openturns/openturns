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
    // Gamma related functions
    {
      // pGamma
      NumericalScalar kMin(0.2);
      NumericalScalar kMax(5.0);
      UnsignedInteger nK(5);
      NumericalScalar xMin(0.1);
      NumericalScalar xMax(0.9);
      UnsignedInteger nX(5);
      for (UnsignedInteger i1 = 0; i1 < nK; ++i1)
      {
        NumericalScalar k(kMin + (kMax - kMin) * i1 / (nK - 1));
        for (UnsignedInteger iX = 0; iX < nX; ++iX)
        {
          NumericalScalar x(xMin + (xMax - xMin) * iX / (nX - 1));
          fullprint << "pGamma(" << k << ", " << x << ")=" << DistFunc::pGamma(k, x) << ", complementary=" << DistFunc::pGamma(k, x, true) << std::endl;
        }
      }
    } // pGamma
    {
      // qGamma
      NumericalScalar kMin(0.2);
      NumericalScalar kMax(5.0);
      UnsignedInteger nK(5);
      NumericalScalar qMin(0.1);
      NumericalScalar qMax(0.9);
      UnsignedInteger nQ(5);
      for (UnsignedInteger i1 = 0; i1 < nK; ++i1)
      {
        NumericalScalar k(kMin + (kMax - kMin) * i1 / (nK - 1));
        for (UnsignedInteger iQ = 0; iQ < nQ; ++iQ)
        {
          NumericalScalar q(qMin + (qMax - qMin) * iQ / (nQ - 1));
          fullprint << "qGamma(" << k << ", " << q << ")=" << DistFunc::qGamma(k, q) << ", complementary=" << DistFunc::qGamma(k, q, true) << std::endl;
        }
      }
    } // qGamma
    {
      // rGamma
      NumericalScalar kMin(0.2);
      NumericalScalar kMax(5.0);
      UnsignedInteger nK(5);
      UnsignedInteger nR(5);
      for (UnsignedInteger i1 = 0; i1 < nK; ++i1)
      {
        NumericalScalar k(kMin + (kMax - kMin) * i1 / (nK - 1));
        for (UnsignedInteger iR = 0; iR < nR; ++iR)
        {
          fullprint << "rGamma(" << k << ")=" << DistFunc::rGamma(k) << std::endl;
        }
      }
    } // rGamma
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}

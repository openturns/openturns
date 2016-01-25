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
#include "OT.hxx"
#include "OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {
    // NonCentralChiSquare related functions
    {
      // dNonCentralChiSquare
      NumericalScalar nuMin(0.2);
      NumericalScalar nuMax(5.0);
      UnsignedInteger n1(5);
      NumericalScalar lambdaMin(0.2);
      NumericalScalar lambdaMax(5.0);
      UnsignedInteger n2(5);
      NumericalScalar xMin(0.1);
      NumericalScalar xMax(0.9);
      UnsignedInteger nX(5);
      for (UnsignedInteger i1 = 0; i1 < n1; ++i1)
      {
        NumericalScalar nu(nuMin + (nuMax - nuMin) * i1 / (n1 - 1));
        for (UnsignedInteger i2 = 0; i2 < n2; ++i2)
        {
          NumericalScalar lambda(lambdaMin + (lambdaMax - lambdaMin) * i2 / (n2 - 1));
          for (UnsignedInteger iX = 0; iX < nX; ++iX)
          {
            NumericalScalar x(xMin + (xMax - xMin) * iX / (nX - 1));
            fullprint << "dNonCentralChiSquare(" << nu << ", " << lambda << ", " << x << ")=" << DistFunc::dNonCentralChiSquare(nu, lambda, x) << std::endl;
          }
        }
      }
    } // dNonCentralChiSquare
    {
      // pNonCentralChiSquare
      NumericalScalar nuMin(0.2);
      NumericalScalar nuMax(5.0);
      UnsignedInteger n1(5);
      NumericalScalar lambdaMin(0.2);
      NumericalScalar lambdaMax(5.0);
      UnsignedInteger n2(5);
      NumericalScalar xMin(0.1);
      NumericalScalar xMax(0.9);
      UnsignedInteger nX(5);
      for (UnsignedInteger i1 = 0; i1 < n1; ++i1)
      {
        NumericalScalar nu(nuMin + (nuMax - nuMin) * i1 / (n1 - 1));
        for (UnsignedInteger i2 = 0; i2 < n2; ++i2)
        {
          NumericalScalar lambda(lambdaMin + (lambdaMax - lambdaMin) * i2 / (n2 - 1));
          for (UnsignedInteger iX = 0; iX < nX; ++iX)
          {
            NumericalScalar x(xMin + (xMax - xMin) * iX / (nX - 1));
            fullprint << "pNonCentralChiSquare(" << nu << ", " << lambda << ", " << x << ")=" << DistFunc::pNonCentralChiSquare(nu, lambda, x) << ", complementary=" << DistFunc::pNonCentralChiSquare(nu, lambda, x, true) << std::endl;
          }
        }
      }
    } // pNonCentralChiSquare
    {
      // rNonCentralChiSquare
      NumericalScalar nuMin(0.2);
      NumericalScalar nuMax(5.0);
      UnsignedInteger n1(5);
      NumericalScalar lambdaMin(0.2);
      NumericalScalar lambdaMax(5.0);
      UnsignedInteger n2(5);
      UnsignedInteger nR(5);
      for (UnsignedInteger i1 = 0; i1 < n1; ++i1)
      {
        NumericalScalar nu(nuMin + (nuMax - nuMin) * i1 / (n1 - 1));
        for (UnsignedInteger i2 = 0; i2 < n2; ++i2)
        {
          NumericalScalar lambda(lambdaMin + (lambdaMax - lambdaMin) * i2 / (n2 - 1));
          for (UnsignedInteger iR = 0; iR < nR; ++iR)
          {
            fullprint << "rNonCentralChiSquare(" << nu << ", " << lambda << ")=" << DistFunc::rNonCentralChiSquare(nu, lambda) << std::endl;
          }
        }
      }
    } // rNonCentralChiSquare
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}

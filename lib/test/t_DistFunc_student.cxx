//                                               -*- C++ -*-
/**
 *  @brief The test file of class DistFunc for standard methods
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
    // Student related functions
    {
      // pStudent
      NumericalScalar nuMin = 0.2;
      NumericalScalar nuMax = 100.0;
      UnsignedInteger n1 = 10;
      NumericalScalar xMin = -10.0;
      NumericalScalar xMax = 10.0;
      UnsignedInteger nX = 20;
      // First, the special cases: small integer nu
      for (UnsignedInteger nu = 1; nu <= 7; ++nu)
      {
        for (UnsignedInteger iX = 0; iX < nX; ++iX)
        {
          NumericalScalar x = xMin + (xMax - xMin) * iX / (nX - 1);
          fullprint << "pStudent(" << nu << ", " << x << ")=" << DistFunc::pStudent(nu, x) << ", complementary=" << DistFunc::pStudent(nu, x, true) << std::endl;
        }
      }
      // Second, the general case: medium real nu
      for (UnsignedInteger i1 = 0; i1 < n1; ++i1)
      {
        NumericalScalar nu = nuMin + (nuMax - nuMin) * i1 / (n1 - 1);
        for (UnsignedInteger iX = 0; iX < nX; ++iX)
        {
          NumericalScalar x = xMin + (xMax - xMin) * iX / (nX - 1);
          fullprint << "pStudent(" << nu << ", " << x << ")=" << DistFunc::pStudent(nu, x) << ", complementary=" << DistFunc::pStudent(nu, x, true) << std::endl;
        }
      }
      // Third, the extreme case: large real nu
      NumericalScalar logNuMin = 9.0;
      NumericalScalar logNuMax = 9.0;
      for (UnsignedInteger i1 = 0; i1 < n1; ++i1)
      {
        NumericalScalar nu = exp(logNuMin + (logNuMax - logNuMin) * i1 / (n1 - 1));
        for (UnsignedInteger iX = 0; iX < nX; ++iX)
        {
          NumericalScalar x = xMin + (xMax - xMin) * iX / (nX - 1);
          fullprint << "pStudent(" << nu << ", " << x << ")=" << DistFunc::pStudent(nu, x) << ", complementary=" << DistFunc::pStudent(nu, x, true) << std::endl;
        }
      }
    } // pStudent
    {
      // qStudent
      NumericalScalar nuMin = 0.2;
      NumericalScalar nuMax = 5.0;
      UnsignedInteger n1 = 5;
      NumericalScalar qMin = 0.1;
      NumericalScalar qMax = 0.9;
      UnsignedInteger nQ = 10;
      // First, the special cases: small integer nu
      for (UnsignedInteger nu = 1; nu <= 6; ++nu)
      {
        for (UnsignedInteger iQ = 0; iQ < nQ; ++iQ)
        {
          NumericalScalar q = qMin + (qMax - qMin) * iQ / (nQ - 1);
          fullprint << "qStudent(" << nu << ", " << q << ")=" << DistFunc::qStudent(nu, q) << ", complementary=" << DistFunc::qStudent(nu, q, true) << std::endl;
        }
      }
      // Second, the general case: medium real nu
      for (UnsignedInteger i1 = 0; i1 < n1; ++i1)
      {
        NumericalScalar nu = nuMin + (nuMax - nuMin) * i1 / (n1 - 1);
        for (UnsignedInteger iQ = 0; iQ < nQ; ++iQ)
        {
          NumericalScalar q = qMin + (qMax - qMin) * iQ / (nQ - 1);
          fullprint << "qStudent(" << nu << ", " << q << ")=" << DistFunc::qStudent(nu, q) << ", complementary=" << DistFunc::qStudent(nu, q, true) << std::endl;
        }
      }
      // Third, the extreme case: large real nu
      NumericalScalar logNuMin = 9.0;
      NumericalScalar logNuMax = 12.0;
      for (UnsignedInteger i1 = 0; i1 < n1; ++i1)
      {
        NumericalScalar nu = exp(logNuMin + (logNuMax - logNuMin) * i1 / (n1 - 1));
        for (UnsignedInteger iQ = 0; iQ < nQ; ++iQ)
        {
          NumericalScalar q = qMin + (qMax - qMin) * iQ / (nQ - 1);
          fullprint << "qStudent(" << nu << ", " << q << ")=" << DistFunc::qStudent(nu, q) << ", complementary=" << DistFunc::qStudent(nu, q, true) << std::endl;
        }
      }
    } // qStudent
    {
      // rStudent
      NumericalScalar nuMin = 0.2;
      NumericalScalar nuMax = 5.0;
      UnsignedInteger n1 = 5;
      UnsignedInteger nR = 10;
      for (UnsignedInteger i1 = 0; i1 < n1; ++i1)
      {
        NumericalScalar nu = nuMin + (nuMax - nuMin) * i1 / (n1 - 1);
        for (UnsignedInteger iR = 0; iR < nR; ++iR)
        {
          fullprint << "rStudent(" << nu << ")=" << DistFunc::rStudent(nu) << std::endl;
        }
      }
    } // rStudent
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}

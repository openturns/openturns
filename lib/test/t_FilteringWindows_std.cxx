//                                               -*- C++ -*-
/**
 *  @brief The test file of class FilteringWindows
 *         This tests the Hamming and Hanning classes
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

  try
  {


    /* Hanning filter */
    Hanning myHanningFilter;
    fullprint << "myHanningFilter = " << myHanningFilter << std::endl;

    /* Hamming filter */
    Hamming myHammingFilter;
    fullprint << "myHammingFilter = " << myHammingFilter << std::endl;

    /* Evaluation of values between t=0 and t=1 using a step = 0.01 */
    const UnsignedInteger    steps(100);
    const NumericalScalar tMin = 0.0;
    const NumericalScalar tMax = 1.0;
    const NumericalScalar tStep = (tMax - tMin) / steps;

    for (UnsignedInteger i = 0; i < steps + 1; ++i )
    {
      const NumericalScalar t = tMin + i * tStep;
      fullprint << "t = " << t
                << " Hanning = " << myHanningFilter(t)
                << " Hamming = " << myHammingFilter(t) << std::endl;
    }

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}

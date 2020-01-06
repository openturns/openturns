//                                               -*- C++ -*-
/**
 *  @brief The test file of class WhiteNoise
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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

    const Scalar Tmin = 0.0;
    const Scalar deltaT = 0.1;
    const UnsignedInteger N = 11;

    /* Initialization of the RegularGrid timeGrid1*/
    RegularGrid timeGrid(Tmin, deltaT, N);

    /* Distribution choice */
    Distribution dist = Uniform();
    fullprint << "dist = " << dist << std::endl;

    Process process = WhiteNoise(dist);

    /* Setting the timeGrid */
    process.setTimeGrid(timeGrid);

    /* Print of the process */
    fullprint << "process = " << process << std::endl;

    /* Initialization of the TimeSeries */
    TimeSeries timeSerie = process.getRealization();

    /* Full prints */
    fullprint << "timeSerie = " << timeSerie << std::endl;

    // Some steps further
    UnsignedInteger stepNumber = 4;
    fullprint << "One future=" << process.getFuture(stepNumber) << std::endl;
    UnsignedInteger size = 3;
    fullprint << "Some futures=" << process.getFuture(stepNumber, size) << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}

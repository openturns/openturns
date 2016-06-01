//                                               -*- C++ -*-
/**
 *  @brief The test file of class LHSExperiment for standard methods
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
    Normal distribution(4);
    UnsignedInteger size(10);
    LHSExperiment myPlane(distribution, size);
    fullprint << "myPlane = " << myPlane << std::endl;
    NumericalPoint weights(0);
    NumericalSample sample(myPlane.generateWithWeights(weights));
    fullprint << "sample  = " << sample << std::endl;
    fullprint << "weights = " << weights << std::endl;
    fullprint << "sample2 = " << myPlane.generate() << std::endl;
    myPlane = LHSExperiment(distribution, size, true, true);
    fullprint << "sample  = " << myPlane.generate() << std::endl;
    fullprint << "sample2 = " << myPlane.generate() << std::endl;
    myPlane = LHSExperiment(distribution, size, false, false);
    fullprint << "sample  = " << myPlane.generate() << std::endl;
    fullprint << "sample2 = " << myPlane.generate() << std::endl;
    myPlane = LHSExperiment(distribution, size, true, false);
    fullprint << "sample  = " << myPlane.generate() << std::endl;
    fullprint << "sample2 = " << myPlane.generate() << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}

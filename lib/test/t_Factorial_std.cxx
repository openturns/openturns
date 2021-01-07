//                                               -*- C++ -*-
/**
 *  @brief The test file of class Factorial for standard methods
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

  try
  {

    Point center(4, 0.0);
    center.setName("center");
    center[0] = 0.5;
    center[1] = 1.5;
    center[2] = 2.5;
    center[3] = 3.5;
    Point levels(3, 0.0);
    levels.setName("levels");
    levels[0] = 4;
    levels[1] = 8;
    levels[2] = 16;
    Factorial experiment(center, levels);
    fullprint << "experiment = " << experiment << std::endl;
    Sample sample(experiment.generate());
    fullprint << "sample = " << sample << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}

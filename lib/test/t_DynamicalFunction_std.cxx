//                                               -*- C++ -*-
/**
 *  @brief The test file of class DynamicalFunction for standard methods
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

  try
  {

    /* Create an intance */
    DynamicalFunction myFunc;

    fullprint << "myFunc=" << myFunc << std::endl;
    /* Get the input and output description */
    fullprint << "myFunc input description=" << myFunc.getInputDescription() << std::endl;
    fullprint << "myFunc output description=" << myFunc.getOutputDescription() << std::endl;
    /* Get the input and output dimension, based on description */
    fullprint << "myFunc input dimension=" << myFunc.getInputDimension() << std::endl;
    fullprint << "myFunc output dimension=" << myFunc.getOutputDimension() << std::endl;
    /* Get the number of calls */
    fullprint << "called " << myFunc.getCallsNumber() << " times" << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}

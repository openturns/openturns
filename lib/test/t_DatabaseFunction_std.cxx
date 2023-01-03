//                                               -*- C++ -*-
/**
 *  @brief The test file of class Function for database
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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

    /* Database construction */
    Sample inputSample(0, 2);
    inputSample.add(Point(2, 1.0));
    inputSample.add(Point(2, 2.0));
    Description inputDescription(0);
    inputDescription.add("x0");
    inputDescription.add("x1");
    inputSample.setDescription(inputDescription);
    Sample outputSample(0, 1);
    outputSample.add(Point(1, 4.0));
    outputSample.add(Point(1, 5.0));
    Description outputDescription(0);
    outputDescription.add("y0");
    outputSample.setDescription(outputDescription);
    DatabaseFunction database(inputSample, outputSample);


    fullprint << "database=" << database.__str__() << std::endl << std::endl;

    /* Does it work? */
    Point x(database.getInputDimension(), 1.8);

    fullprint << "x=" << x.__str__() << std::endl;
    fullprint << "database(x)=" << database(x).__str__() << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}

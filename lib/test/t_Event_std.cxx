//                                               -*- C++ -*-
/**
 *  @brief The test file of Event class
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

    /* We create a numerical point of dimension 1 */
    Point point(4);
    const Point & ref_point(point);

    point[0] = 101;
    point[1] = 202;
    point[2] = 303;
    point[3] = 404;

    fullprint << "point = " << ref_point << std::endl;

    /* We create a 'constant' RandomVector from the Point */
    ConstantRandomVector vect(point);
    fullprint << "vect=" << vect << std::endl;

    /* Check standard methods of class RandomVector */
    fullprint << "vect dimension=" << vect.getDimension() << std::endl;
    fullprint << "vect realization (first )=" << vect.getRealization() << std::endl;
    fullprint << "vect sample =" << vect.getSample(5) << std::endl;

    /* We create a numerical math function */
    Description input(4);
    input[0] = "E";
    input[1] = "F";
    input[2] = "L";
    input[3] = "I";
    SymbolicFunction myFunction(input, Description(1, "-F*L^3/(3*E*I)"));

    /* We create a composite random vector */
    CompositeRandomVector output(myFunction, vect);

    /* We create an Event from this RandomVector */
    ThresholdEvent myEvent(output, Less(), 50);
    fullprint << "myEvent=" << myEvent << std::endl;

    /* We compute one realization of the event */
    fullprint << "myEvent realization=" << myEvent.getRealization() << std::endl;
    fullprint << "myEvent antecedent realization=" << myEvent.getAntecedent().getRealization() << std::endl;

    /* We compute a sample of the event */
    fullprint << "myEvent sample=" << myEvent.getSample(10) << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}

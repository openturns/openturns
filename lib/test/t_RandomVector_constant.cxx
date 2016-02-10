//                                               -*- C++ -*-
/**
 *  @brief The test file of 'constant' RandomVector class
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

    /* We create a numerical point of dimension 4 */
    NumericalPoint point(4);
    const NumericalPoint & ref_point(point);

    point[0] = 101.0;
    point[1] = 102.0;
    point[2] = 103.0;
    point[3] = 104.0;

    fullprint << "point = " << ref_point << std::endl;

    /* We create a 'constant' RandomVector from the NumericalPoint */
    RandomVector vect = ConstantRandomVector(point);
    fullprint << "vect=" << vect << std::endl;

    /* Check standard methods of class RandomVector */
    fullprint << "vect dimension=" << vect.getDimension() << std::endl;
    fullprint << "vect realization (first )=" << vect.getRealization() << std::endl;
    fullprint << "vect realization (second)=" << vect.getRealization() << std::endl;
    fullprint << "vect realization (third )=" << vect.getRealization() << std::endl;
    fullprint << "vect sample =" << vect.getSample(5) << std::endl;

    /* We create a numerical point of dimension 4 */
    NumericalPointWithDescription pointWithDescription(4);
    pointWithDescription[0] = 101.0;
    pointWithDescription[1] = 102.0;
    pointWithDescription[2] = 103.0;
    pointWithDescription[3] = 104.0;
    Description description(4);
    description[0] = "x0";
    description[1] = "x1";
    description[2] = "x2";
    description[3] = "x3";
    pointWithDescription.setDescription(description);

    fullprint << "point with description= " << pointWithDescription << std::endl;

    /* We create a 'constant' RandomVector from the NumericalPoint */
    RandomVector vect2 = ConstantRandomVector(pointWithDescription);
    fullprint << "vect2=" << vect2 << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}

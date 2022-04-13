//                                               -*- C++ -*-
/**
 *  @brief The test file of class Point for standard methods
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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

  /* Constructor with size */
  PointWithDescription point1(2);
  point1[0] = 100.;
  point1[1] = 101.;

  /* Copy constructor */
  PointWithDescription point2(point1);


  {
    /* Description */
    fullprint << "point1.getDescription() = "
              << point1.getDescription()
              << std::endl;

    fullprint << "point2.getDescription() = "
              << point2.getDescription()
              << std::endl;
  }



  Description description(2);
  description[0] = "point1_X";
  description[1] = "point1_Y";

  point1.setDescription(description);


  {
    fullprint << "point1.getDescription() = "
              << point1.getDescription()
              << std::endl;

    fullprint << "point2.getDescription() = "
              << point2.getDescription()
              << std::endl;
  }

  // Creation
  Collection<PointWithDescription> coll1(5, PointWithDescription(4));
  Collection<PointWithDescription> coll2(5, Point(4));
  Collection<Point> coll3(5, PointWithDescription(4));
  Collection<Point> coll4(5, Point(4));
  // Conversion
  //coll1 = coll3;
  //coll4 = coll2;
  return ExitCode::Success;
}

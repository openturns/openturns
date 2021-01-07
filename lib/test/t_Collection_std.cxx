//                                               -*- C++ -*-
/**
 *  @brief The test file of class Collection for standard methods
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

  /* Default constructor */
  Collection<double> point1;

  /* Check method add() */
  point1.add(0.);
  point1.add(1.);

  fullprint << "size of point1 = " << point1.getSize() << std::endl;

  fullprint << "point1[0] = " << point1[0] << std::endl
            << "point1[1] = " << point1[1] << std::endl;


  /* Constructor with size */
  Collection<double> point2(2);

  /* Check operator[] methods */
  point2[0] = 10.;
  point2[1] = 11.;

  fullprint << "point2[0] = " << point2[0] << std::endl
            << "point2[1] = " << point2[1] << std::endl;


  /* Copy constructor */
  Collection<double> point3(point1);

  fullprint << "point3[0] = " << point3[0] << std::endl
            << "point3[1] = " << point3[1] << std::endl;


  /* Assignment operator */
  Collection<double> point4;
  point4 = point2;

  fullprint << "point4[0] = " << point4[0] << std::endl
            << "point4[1] = " << point4[1] << std::endl;

  /* Stream operator */
  fullprint << "point1 = " << point1 << std::endl;

  /* Constructor with size and value */
  Collection<double> point5(5, 1.0);
  fullprint << "point5 = " << point5 << std::endl;

  /* Large collection, the size is printed */
  Collection<double> point6(12, 1.5);
  fullprint << "point6 = " << point6 << std::endl;

  return ExitCode::Success;
}

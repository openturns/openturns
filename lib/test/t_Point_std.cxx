//                                               -*- C++ -*-
/**
 *  @brief The test file of class Point for standard methods
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

  try
  {
    Scalar val1, val2;

    /* Default constructor */
    Point point1;
    const Point & ref_point1(point1);

    /* Check method add() */
    point1.add(0.);
    point1.add(1.);

    UnsignedInteger size = ref_point1.getDimension();
    fullprint << "size of point1 = " << size << std::endl;

    val1 = ref_point1[0];
    val2 = ref_point1[1];
    fullprint << "point1 = " << ref_point1 << std::endl;
    fullprint << "point1[0] = " << val1 << std::endl;
    fullprint << "point1[1] = " << val2 << std::endl;


    /* Constructor with size */
    Point point2(2);
    const Point & ref_point2(point2);

    /* Check operator[] methods */
    point2[0] = 10.;
    point2[1] = 11.;

    val1 = ref_point2[0];
    val2 = ref_point2[1];
    fullprint << "point2[0] = " << val1 << std::endl;
    fullprint << "point2[1] = " << val2 << std::endl;


    /* Copy constructor */
    Point point3(ref_point1);
    const Point & ref_point3(point3);

    val1 = ref_point3[0];
    val2 = ref_point3[1];
    fullprint << "point3[0] = " << val1 << std::endl;
    fullprint << "point3[1] = " << val2 << std::endl;


    /* Assignment operator */
    Point point4;
    const Point & ref_point4(point4);
    point4 = ref_point2;

    val1 = ref_point4[0];
    val2 = ref_point4[1];
    fullprint << "point4[0] = " << val1 << std::endl;
    fullprint << "point4[1] = " << val2 << std::endl;


    /* Comparison operator */
    if (! (ref_point2 == ref_point2))
    {
      OSS oss;
      oss << "OT::Point.operator == does NOT return the correct value. Says that point2 and point2 are DIFFERENT though they are equal.";
      throw TestFailed(oss);
    }

    if (! (ref_point2 == ref_point4))
    {
      OSS oss;
      oss << "OT::Point.operator == does NOT return the correct value. Says that point2 and point4 are DIFFERENT though they are equal.";
      throw TestFailed(oss);
    }

    if (ref_point2 == ref_point3)
    {
      OSS oss;
      oss << "OT::Point.operator == does NOT return the correct value. Says that point2 and point3 are EQUAL though they are different.";
      throw TestFailed(oss);
    }



    /* Addition/Subtraction/Product operators */
    try
    {

      Point point5 = ref_point1 + ref_point2;
      const Point & ref_point5(point5);
      fullprint << "point5 = " << ref_point5 << std::endl;

      Point point6 = ref_point1 - ref_point2;
      const Point & ref_point6(point6);
      fullprint << "point6 = " << ref_point6 << std::endl;

      Point point7(ref_point5);
      point7 += ref_point1 + ref_point2;
      const Point & ref_point7(point7);
      fullprint << "point7 = " << ref_point7 << std::endl;

      Point point8(ref_point6);
      point8 -= ref_point1 - ref_point2;
      const Point & ref_point8(point8);
      fullprint << "point8 = " << ref_point8 << std::endl;


      // We create an orthonormal base (O,i,j)
      Point i(2);
      i[0] = 1;
      i[1] = 0;
      fullprint << "i = " << i << std::endl;

      Point j(2);
      j[0] = 0;
      j[1] = 1;
      fullprint << "j = " << j << std::endl;

      Point I(2. * i);
      fullprint << "I = " << I << std::endl;

      Point J(j * 3.);
      fullprint << "J = " << J << std::endl;

      Scalar dotProduct = I.dot(J);
      fullprint << "dotProduct = " << dotProduct << std::endl;

    }
    catch (InvalidArgumentException & ex)
    {
      throw TestFailed(ex.__repr__());
    }


    /* sort method */
    try
    {

      Point point9(3, 0);
      point9[1] = -1;
      point9[2] = -2;
      Point point10(point9);
      std::sort(point10.begin(), point10.end());
      fullprint << "point9 " << point9 << std::endl;
      fullprint << "point10 " << point10 << std::endl;
    }
    catch (InvalidArgumentException & ex)
    {
      throw TestFailed(ex.__repr__());
    }

    // empty point operators test
    {
      Point point0;
      Point point0mul(point0 * 5.0);
      Point point0div(point0 / 2.0);
      point0 *= 5.0;
      point0 /= 2.0;
      fullprint << "point0=" << point0 << std::endl;
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}

//                                               -*- C++ -*-
/**
 *  @brief The test file of class NumericalPoint for standard methods
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

  try
  {
    NumericalScalar val1, val2;

    /* Default constructor */
    NumericalPoint point1;
    const NumericalPoint & ref_point1(point1);

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
    NumericalPoint point2(2);
    const NumericalPoint & ref_point2(point2);

    /* Check operator[] methods */
    point2[0] = 10.;
    point2[1] = 11.;

    val1 = ref_point2[0];
    val2 = ref_point2[1];
    fullprint << "point2[0] = " << val1 << std::endl;
    fullprint << "point2[1] = " << val2 << std::endl;


    /* Copy constructor */
    NumericalPoint point3(ref_point1);
    const NumericalPoint & ref_point3(point3);

    val1 = ref_point3[0];
    val2 = ref_point3[1];
    fullprint << "point3[0] = " << val1 << std::endl;
    fullprint << "point3[1] = " << val2 << std::endl;


    /* Assignment operator */
    NumericalPoint point4;
    const NumericalPoint & ref_point4(point4);
    point4 = ref_point2;

    val1 = ref_point4[0];
    val2 = ref_point4[1];
    fullprint << "point4[0] = " << val1 << std::endl;
    fullprint << "point4[1] = " << val2 << std::endl;


    /* Comparison operator */
    if (! (ref_point2 == ref_point2))
    {
      OSS oss;
      oss << "OT::NumericalPoint.operator == does NOT return the correct value. Says that point2 and point2 are DIFFERENT though they are equal.";
      throw TestFailed(oss);
    }

    if (! (ref_point2 == ref_point4))
    {
      OSS oss;
      oss << "OT::NumericalPoint.operator == does NOT return the correct value. Says that point2 and point4 are DIFFERENT though they are equal.";
      throw TestFailed(oss);
    }

    if (ref_point2 == ref_point3)
    {
      OSS oss;
      oss << "OT::NumericalPoint.operator == does NOT return the correct value. Says that point2 and point3 are EQUAL though they are different.";
      throw TestFailed(oss);
    }



    /* Addition/Substraction/Product operators */
    try
    {

      NumericalPoint point5 = ref_point1 + ref_point2;
      const NumericalPoint & ref_point5(point5);
      fullprint << "point5 = " << ref_point5 << std::endl;

      NumericalPoint point6 = ref_point1 - ref_point2;
      const NumericalPoint & ref_point6(point6);
      fullprint << "point6 = " << ref_point6 << std::endl;

      NumericalPoint point7(ref_point5);
      point7 += ref_point1 + ref_point2;
      const NumericalPoint & ref_point7(point7);
      fullprint << "point7 = " << ref_point7 << std::endl;

      NumericalPoint point8(ref_point6);
      point8 -= ref_point1 - ref_point2;
      const NumericalPoint & ref_point8(point8);
      fullprint << "point8 = " << ref_point8 << std::endl;


      // We create an orthonormal base (O,i,j)
      NumericalPoint i(2);
      i[0] = 1;
      i[1] = 0;
      fullprint << "i = " << i << std::endl;

      NumericalPoint j(2);
      j[0] = 0;
      j[1] = 1;
      fullprint << "j = " << j << std::endl;

      NumericalPoint I(2. * i);
      fullprint << "I = " << I << std::endl;

      NumericalPoint J(j * 3.);
      fullprint << "J = " << J << std::endl;

      NumericalScalar dotProduct = dot(I, J);
      fullprint << "dotProduct = " << dotProduct << std::endl;

    }
    catch (InvalidArgumentException & ex)
    {
      throw TestFailed(ex.__repr__());
    }


    /* sort method */
    try
    {

      NumericalPoint point9(3, 0);
      point9[1] = -1;
      point9[2] = -2;
      NumericalPoint point10(point9);
      std::sort(point10.begin(), point10.end());
      fullprint << "point9 " << point9 << std::endl;
      fullprint << "point10 " << point10 << std::endl;

    }
    catch (InvalidArgumentException & ex)
    {
      throw TestFailed(ex.__repr__());
    }

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}

//                                               -*- C++ -*-
/**
 *  @brief The test file of class ARMACoefficients
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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

typedef Collection<SquareMatrix> SquareMatrixCollection;

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {

    /* Constructor with size */
    const UnsignedInteger dim = 2;
    SquareMatrix squareMatrix1(dim);
    squareMatrix1(0, 0) = 1.;
    squareMatrix1(1, 0) = 2.;
    squareMatrix1(0, 1) = 3.;
    squareMatrix1(1, 1) = 4.;


    /* Second matrix to add to the ARMACoefficients*/
    double s = 3.0;
    SquareMatrix squareMatrix2 = squareMatrix1 * s;

    /* Last matrix to add to the ARMACoefficients*/
    double t = 1.5;
    SquareMatrix squareMatrix3 = squareMatrix1 / t;

    /* size : Number of matrix */
    const UnsignedInteger size = 3;

    /* ARMACoefficients with default constructor */
    ARMACoefficients coefficients0;
    fullprint << "Using default constructor " << std::endl;
    fullprint << "coefficients0 = " << coefficients0 << std::endl;

    /* ARMACoefficients with size / dimension constructor */
    ARMACoefficients coefficients1(size, dim);
    coefficients1[0] = squareMatrix1 ;
    coefficients1[1] = squareMatrix2 ;
    coefficients1[2] = squareMatrix3 ;

    fullprint << "Using constructor based on size / dimension " << std::endl;
    fullprint << "coefficients1 = " << coefficients1 << std::endl;
    fullprint << "size of coefficients1 = " << coefficients1.getSize() << std::endl;
    fullprint << "dimension of coefficients1 objects= " << coefficients1.getDimension() << std::endl;

    /*constructors with collection of matrix*/
    SquareMatrixCollection myCollection;
    myCollection.add(squareMatrix1);
    myCollection.add(squareMatrix2);
    myCollection.add(squareMatrix3);

    fullprint << "SquareMatrix collection = " << myCollection.__repr__() << std::endl;

    /*fullprint of the new collection*/
    fullprint << "Using constructor based on a collection " << std::endl;
    ARMACoefficients coefficients2(myCollection);
    fullprint << "coefficients2 = " << coefficients2 << std::endl;

    /* 1D case */
    Point point(dim * dim, 1.0);
    point[0] = 1;
    point[1] = 2;
    point[2] = 3;
    point[3] = 4;

    /* ARMACoefficients Point constructor */
    ARMACoefficients coefficients3(point);
    coefficients3.add(s);

    /*fullprint of the new collection*/
    fullprint << "Using Point constructor " << std::endl;
    fullprint << "coefficients3 = " << coefficients3 << std::endl;

    /* ARMACoefficients with polynomial constructor */
    point *= 2.0 ;
    UniVariatePolynomial poly(point);
    ARMACoefficients coefficients4(poly);

    /*fullprint of the new collection*/
    fullprint << "Using polynomial constructor " << std::endl;
    fullprint << "coefficients4 = " << coefficients4 << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}

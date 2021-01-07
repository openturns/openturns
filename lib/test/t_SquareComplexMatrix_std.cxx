//                                               -*- C++ -*-
/**
 *  @brief The test file of class SquareComplexMatrix for standard methods
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

typedef Collection<Complex> ComplexCollection;

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {

    /** DEFAULT CONSTRUCTOR AND STRING CONVERTER */
    fullprint << "test 0 : default constructor and string converter" << std::endl;

    /* Default constructor */
    SquareComplexMatrix squareComplexMatrix0;

    /* String converter */
    fullprint << "squareComplexMatrix0 = " << squareComplexMatrix0 << std::endl;


    /** CONSTRUCTOR WITH SIZE, OPERATOR() AND STRING CONVERTER */
    fullprint << "test 1: constructor with size, operator() and string converter" << std::endl;

    /* Constructor with size */
    SquareComplexMatrix squareComplexMatrix1(2);

    /* Check operator() methods */
    squareComplexMatrix1(0, 0) = Complex(1.0, 1.0) ;
    squareComplexMatrix1(0, 1) = Complex(3.0, 1.0) ;
    squareComplexMatrix1(1, 0) = Complex(0.0, 1.0) ;
    squareComplexMatrix1(1, 1) = Complex(5.0, 1.0) ;

    /* String converter */
    fullprint << "squareComplexMatrix1 = " << squareComplexMatrix1 << std::endl;


    /** COPY CONSTRUCTOR AND STRING CONVERTER */
    fullprint << "test 2 : copy constructor and string converter" << std::endl;

    /* Copy constructor */
    SquareComplexMatrix squareComplexMatrix2(squareComplexMatrix1);

    /* String converter */
    fullprint << "squareComplexMatrix2 = " << squareComplexMatrix2 << std::endl;


    /** GET DIMENSIONS METHODS */
    fullprint << "test 3 : get dimensions methods" << std::endl;

    /* Get dimension methods */
    fullprint << "squareComplexMatrix1's dimension = " << squareComplexMatrix1.getDimension() << std::endl;


    /**  ASSIGNMENT METHOD */
    fullprint << "test 4 : assignment method" << std::endl;

    /* Assignment method */
    SquareComplexMatrix squareComplexMatrix3 ;
    squareComplexMatrix3 = squareComplexMatrix1 ;
    fullprint << "squareComplexMatrix3 = " << squareComplexMatrix3 << std::endl;

    /** CONJUGATE METHOD */
    fullprint << "test 5 : conjugate method" << std::endl;

    /* Check transpose method */
    SquareComplexMatrix squareComplexMatrix4 = squareComplexMatrix1.conjugate();
    fullprint << "squareComplexMatrix1 conjugate = " << squareComplexMatrix4 << std::endl;

    /**  ADDITION METHOD */
    fullprint << "test number 6 : addition method" << std::endl;

    /* Check addition method : we check the operator and the symmetry of the operator, thus testing the comparison operator */
    SquareComplexMatrix sum1 = squareComplexMatrix1.operator + ( squareComplexMatrix4 ) ;
    SquareComplexMatrix sum2 = squareComplexMatrix4.operator + ( squareComplexMatrix1 ) ;
    fullprint << "sum1 = " << sum1 << std::endl;
    fullprint << "sum2 = " << sum2 << std::endl;
    fullprint << "sum1 equals sum2 = " << (sum1 == sum2) << std::endl;


    /** SUBTRACTION METHOD */
    fullprint << "test 7 : subtraction method" << std::endl;

    /* Check subtraction method */
    SquareComplexMatrix diff = squareComplexMatrix1.operator - ( squareComplexMatrix4 ) ;
    fullprint << "diff = " << diff << std::endl;

    /**  MATRIX MULTIPLICATION METHOD */
    fullprint << "test 8 : matrix multiplication method" << std::endl;

    /* Check multiplication method */
    SquareComplexMatrix prod = squareComplexMatrix1.operator * ( squareComplexMatrix4 ) ;
    fullprint << "prod = " << prod << std::endl;

    /**  MULTIPLICATION WITH A NUMERICAL POINT METHOD */
    fullprint << "test 9 : multiplication with a numerical point method" << std::endl;

    /* Create the numerical point */
    ComplexCollection pt(2) ;
    pt[0] = Complex(1.0, 1.0)  ;
    pt[1] = Complex(0.0, 1.0) ;
    fullprint << "pt = " << pt << std::endl;

    /* Check the product method */
    ComplexCollection ptResult = squareComplexMatrix1.operator * ( pt )  ;
    fullprint << "ptResult = " << ptResult << std::endl;

    /** MULTIPLICATION AND DIVISION BY A NUMERICAL SCALAR METHODS */
    fullprint << "test 10 : multiplication and division by a numerical scalar methods" << std::endl;

    /* Check the multiplication method */
    Complex s(2.0, 1.0);
    SquareComplexMatrix scalprod1 = squareComplexMatrix1.operator * (s) ;
    SquareComplexMatrix scalprod2 = s * squareComplexMatrix1 ;
    fullprint << "scalprod1 = " << scalprod1 << std::endl;
    fullprint << "scalprod2 = " << scalprod2 << std::endl;
    fullprint << "scalprod1 equals scalprod2 = " << (scalprod1 == scalprod2) << std::endl;

    /* Check the division method */
    SquareComplexMatrix scaldiv1 = squareComplexMatrix1.operator / (s) ;
    SquareComplexMatrix scaldiv2 = squareComplexMatrix1.operator * (1.0 / s) ;
    fullprint << "scaldiv1 = " << scaldiv1 << std::endl;
    fullprint << "scaldiv2 = " << scaldiv2 << std::endl;
    fullprint << "scaldiv1 equals scaldiv2 = " << (scaldiv1 == scaldiv2) << std::endl;


    /** ISEMPTY METHOD */
    fullprint << "test number twelve : isEmpty method" << std::endl;

    /* Check method isEmpty */
    SquareComplexMatrix squareComplexMatrix5;
    fullprint << "squareComplexMatrix0 is empty = " << squareComplexMatrix0.isEmpty() << std::endl
              << "squareComplexMatrix1 is empty = " << squareComplexMatrix1.isEmpty() << std::endl
              << "squareComplexMatrix5 is empty = " << squareComplexMatrix5.isEmpty() << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}

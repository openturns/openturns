//                                               -*- C++ -*-
/**
 *  @brief The test file of class SquareMatrix for standard methods
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

  try
  {

    /** TEST NUMBER ZERO : DEFAULT CONSTRUCTOR AND STRING CONVERTER */
    fullprint << "test number zero : default constructor and string converter" << std::endl;

    /* Default constructor */
    SquareMatrix squareMatrix0;

    /* String converter */
    fullprint << "squareMatrix0 = " << squareMatrix0 << std::endl;


    /** TEST NUMBER ONE : CONSTRUCTOR WITH SIZE, OPERATOR() AND STRING CONVERTER */
    fullprint << "test number one : constructor with size, operator() and string converter" << std::endl;

    /* Constructor with size */
    SquareMatrix squareMatrix1(2);

    /* Check operator() methods */
    squareMatrix1(0, 0) = 1. ;
    squareMatrix1(1, 0) = 2. ;
    squareMatrix1(0, 1) = 3. ;
    squareMatrix1(1, 1) = 4. ;

    /* String converter */
    fullprint << "squareMatrix1 = " << squareMatrix1 << std::endl;

    /** TEST NUMBER TWO : COPY CONSTRUCTOR AND STRING CONVERTER */
    fullprint << "test number two : copy constructor and string converter" << std::endl;

    /* Copy constructor */
    SquareMatrix squareMatrix2(squareMatrix1);

    /* String converter */
    fullprint << "squareMatrix2 = " << squareMatrix2 << std::endl;


    /** TEST NUMBER THREE : GET DIMENSIONS METHODS */
    fullprint << "test number three : get dimensions methods" << std::endl;

    /* Get dimension methods */
    fullprint << "squareMatrix1's nbRows = " << squareMatrix1.getNbRows() << std::endl
              << "squareMatrix1's nbColumns = " << squareMatrix1.getNbColumns() << std::endl;

#if 0
    /** TEST NUMBER FOUR : SET DIMENSIONS METHOD */
    fullprint << "test number four : set dimensions method" << std::endl;

    /* Set dimension methods */
    squareMatrix0.setUniqueDimension(2);
    fullprint << "squareMatrix0's nbRows = " << squareMatrix0.getNbRows() << std::endl
              << "squareMatrix0's nbColumns = " << squareMatrix0.getNbColumns() << std::endl;
#endif


    /** TEST NUMBER FIVE : ASSIGNMENT METHOD */
    fullprint << "test number five : assignment method" << std::endl;

    /* Assignment method */
    SquareMatrix squareMatrix3 ;
    squareMatrix3 = squareMatrix1 ;
    fullprint << "squareMatrix3 = " << squareMatrix3 << std::endl;

    /** TEST NUMBER SIX : TRANSPOSITION METHOD */
    fullprint << "test number six : transposition method" << std::endl;

    /* Check transpose method */
    SquareMatrix squareMatrix4 = squareMatrix1.transpose();
    fullprint << "squareMatrix1 transposed = " << squareMatrix4 << std::endl;
    fullprint << "squareMatrix1 transposed = " << squareMatrix1.transpose() << std::endl;

    /** TEST NUMBER SEVEN : ADDITION METHOD */
    fullprint << "test number seven : addition method" << std::endl;

    /* Check addition method : we check the operator and the symmetry of the operator, thus testing the comparison operator */
    SquareMatrix sum1 = squareMatrix1.operator + ( squareMatrix4 ) ;
    SquareMatrix sum2 = squareMatrix4.operator + ( squareMatrix1 ) ;
    fullprint << "sum1 = " << sum1 << std::endl;
    fullprint << "sum2 = " << sum2 << std::endl;
    fullprint << "sum1 equals sum2 = " << (sum1 == sum2) << std::endl;


    /** TEST NUMBER EIGHT : SUBTRACTION METHOD */
    fullprint << "test number eight : subtraction method" << std::endl;

    /* Check subtraction method */
    SquareMatrix diff = squareMatrix1.operator - ( squareMatrix4 ) ;
    fullprint << "diff = " << diff << std::endl;


    /** TEST NUMBER NINE : MATRIX MULTIPLICATION METHOD */
    fullprint << "test number nine : matrix multiplication method" << std::endl;

    /* Check multiplication method */
    SquareMatrix prod = squareMatrix1.operator * ( squareMatrix4 ) ;
    fullprint << "prod = " << prod << std::endl;


    /** TEST NUMBER TEN : MULTIPLICATION WITH A NUMERICAL POINT METHOD */
    fullprint << "test number ten : multiplication with a numerical point method" << std::endl;

    /* Create the numerical point */
    Point pt ;
    pt.add(1.) ;
    pt.add(2.) ;
    fullprint << "pt = " << pt << std::endl;

    /* Check the product method */
    Point ptResult = squareMatrix1.operator * ( pt )  ;
    fullprint << "ptResult = " << ptResult << std::endl;

    /** TEST NUMBER ELEVEN : MULTIPLICATION AND DIVISION BY A NUMERICAL SCALAR METHODS */
    fullprint << "test number eleven : multiplication and division by a numerical scalar methods" << std::endl;

    /* Check the multiplication method */
    double s = 3.;
    SquareMatrix scalprod1 = squareMatrix1.operator * (s) ;
    SquareMatrix scalprod2 = s * squareMatrix1 ;
    fullprint << "scalprod1 = " << scalprod1 << std::endl;
    fullprint << "scalprod2 = " << scalprod2 << std::endl;
    fullprint << "scalprod1 equals scalprod2 = " << (scalprod1 == scalprod2) << std::endl;

    /* Check the division method */
    SquareMatrix scaldiv1 = squareMatrix1.operator / (s) ;
    SquareMatrix scaldiv2 = squareMatrix1.operator * (1 / s) ;
    fullprint << "scaldiv1 = " << scaldiv1 << std::endl;
    fullprint << "scaldiv2 = " << scaldiv2 << std::endl;
    fullprint << "scaldiv1 equals scaldiv2 = " << (scaldiv1 == scaldiv2) << std::endl;


    /** TEST NUMBER TWELVE : ISEMPTY METHOD */
    fullprint << "test number twelve : isEmpty method" << std::endl;

    /* Check method isEmpty */
    SquareMatrix squareMatrix5;
    fullprint << "squareMatrix0 is empty = " << squareMatrix0.isEmpty() << std::endl
              << "squareMatrix1 is empty = " << squareMatrix1.isEmpty() << std::endl
              << "squareMatrix5 is empty = " << squareMatrix5.isEmpty() << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}

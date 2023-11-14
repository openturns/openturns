//                                               -*- C++ -*-
/**
 *  @brief The test file of class SymmetricMatrix for standard methods
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
    SymmetricMatrix SymmetricMatrix0;

    /* String converter */
    fullprint << "SymmetricMatrix0 = " << SymmetricMatrix0 << std::endl;


    /** TEST NUMBER ONE : CONSTRUCTOR WITH SIZE, OPERATOR() AND STRING CONVERTER */
    fullprint << "test number one : constructor with size, operator() and string converter" << std::endl;

    /* Constructor with size */
    SymmetricMatrix SymmetricMatrix1(2);

    /* Check operator() methods */
    SymmetricMatrix1(0, 0) = 1. ;
    SymmetricMatrix1(1, 0) = 2. ;
    SymmetricMatrix1(0, 1) = 3. ;
    SymmetricMatrix1(1, 1) = 4. ;

    /* String converter */
    fullprint << "SymmetricMatrix1 = " << SymmetricMatrix1 << std::endl;
    fullprint << "SymmetricMatrix1 = " << SymmetricMatrix1.__str__() << std::endl;


    /** TEST NUMBER TWO : COPY CONSTRUCTOR AND STRING CONVERTER */
    fullprint << "test number two : copy constructor and string converter" << std::endl;

    /* Copy constructor */
    SymmetricMatrix SymmetricMatrix2(SymmetricMatrix1);

    /* String converter */
    fullprint << "SymmetricMatrix2 = " << SymmetricMatrix2 << std::endl;
    fullprint << "SymmetricMatrix2 = " << SymmetricMatrix2.__str__() << std::endl;


    /** TEST NUMBER THREE : GET DIMENSIONS METHODS */
    fullprint << "test number three : get dimensions methods" << std::endl;

    /* Get dimension methods */
    fullprint << "SymmetricMatrix1's nbRows = " << SymmetricMatrix1.getNbRows() << std::endl
              << "SymmetricMatrix1's nbColumns = " << SymmetricMatrix1.getNbColumns() << std::endl;

#if 0
    /** TEST NUMBER FOUR : SET DIMENSIONS METHOD */
    fullprint << "test number four : set dimensions method" << std::endl;

    /* Set dimension methods */
    SymmetricMatrix0.setUniqueDimension(2);
    fullprint << "SymmetricMatrix0's nbRows = " << SymmetricMatrix0.getNbRows() << std::endl
              << "SymmetricMatrix0's nbColumns = " << SymmetricMatrix0.getNbColumns() << std::endl;
#endif


    /** TEST NUMBER FIVE : ASSIGNMENT METHOD */
    fullprint << "test number five : assignment method" << std::endl;

    /* Assignment method */
    SymmetricMatrix SymmetricMatrix3 ;
    SymmetricMatrix3 = SymmetricMatrix1 ;
    fullprint << "SymmetricMatrix3 = " << SymmetricMatrix3 << std::endl;
    fullprint << "SymmetricMatrix3 = " << SymmetricMatrix3.__str__() << std::endl;

    /** TEST NUMBER SIX : TRANSPOSITION METHOD */
    fullprint << "test number six : transposition method" << std::endl;

    /* Check transpose method */
    SymmetricMatrix SymmetricMatrix4 = SymmetricMatrix1.transpose();
    fullprint << "SymmetricMatrix1 transposed = " << SymmetricMatrix4 << std::endl;
    fullprint << "SymmetricMatrix1 transposed = " << SymmetricMatrix4.__str__() << std::endl;
    fullprint << "SymmetricMatrix1 transposed = " << SymmetricMatrix1.transpose() << std::endl;

    /** TEST NUMBER SEVEN : ADDITION METHOD */
    fullprint << "test number seven : addition method" << std::endl;

    /* Check addition method : we check the operator and the symmetry of the operator, thus testing the comparison operator */
    SymmetricMatrix sum1 = SymmetricMatrix1.operator + ( SymmetricMatrix4 ) ;
    SymmetricMatrix sum2 = SymmetricMatrix4.operator + ( SymmetricMatrix1 ) ;
    fullprint << "sum1 = " << sum1 << std::endl;
    fullprint << "sum1 = " << sum1.__str__() << std::endl;
    fullprint << "sum2 = " << sum2 << std::endl;
    fullprint << "sum2 = " << sum2.__str__() << std::endl;
    fullprint << "sum1 equals sum2 = " << (sum1 == sum2) << std::endl;


    /** TEST NUMBER EIGHT : SUBTRACTION METHOD */
    fullprint << "test number eight : subtraction method" << std::endl;

    /* Check subtraction method */
    SymmetricMatrix diff = SymmetricMatrix1.operator - ( SymmetricMatrix4 ) ;
    fullprint << "diff = " << diff << std::endl;
    fullprint << "diff = " << diff.__str__() << std::endl;


    /** TEST NUMBER NINE : MATRIX MULTIPLICATION METHOD */
    fullprint << "test number nine : matrix multiplication method" << std::endl;

    /* Check multiplication method */
    SquareMatrix prod = SymmetricMatrix1.operator * ( SymmetricMatrix4 ) ;
    fullprint << "prod = " << prod << std::endl;
    fullprint << "prod = " << prod.__str__() << std::endl;


    /** TEST NUMBER TEN : MULTIPLICATION WITH A NUMERICAL POINT METHOD */
    fullprint << "test number ten : multiplication with a numerical point method" << std::endl;

    /* Create the numerical point */
    Point pt ;
    pt.add(1.) ;
    pt.add(2.) ;
    fullprint << "pt = " << pt << std::endl;

    /* Check the product method */
    Point ptResult = SymmetricMatrix1.operator * ( pt )  ;
    fullprint << "ptResult = " << ptResult << std::endl;

    /** TEST NUMBER ELEVEN : MULTIPLICATION AND DIVISION BY A NUMERICAL SCALAR METHODS */
    fullprint << "test number eleven : multiplication and division by a numerical scalar methods" << std::endl;

    /* Check the multiplication method */
    double s = 3.;
    SymmetricMatrix scalprod1 = SymmetricMatrix1.operator * (s) ;
    SymmetricMatrix scalprod2 = s * SymmetricMatrix1 ;
    fullprint << "scalprod1 = " << scalprod1 << std::endl;
    fullprint << "scalprod1 = " << scalprod1.__str__() << std::endl;
    fullprint << "scalprod2 = " << scalprod2 << std::endl;
    fullprint << "scalprod2 = " << scalprod2.__str__() << std::endl;
    fullprint << "scalprod1 equals scalprod2 = " << (scalprod1 == scalprod2) << std::endl;

    /* Check the division method */
    SymmetricMatrix scaldiv1 = SymmetricMatrix1.operator / (s) ;
    SymmetricMatrix scaldiv2 = SymmetricMatrix1.operator * (1 / s) ;
    fullprint << "scaldiv1 = " << scaldiv1 << std::endl;
    fullprint << "scaldiv1 = " << scaldiv1.__str__() << std::endl;
    fullprint << "scaldiv2 = " << scaldiv2 << std::endl;
    fullprint << "scaldiv2 = " << scaldiv2.__str__() << std::endl;
    fullprint << "scaldiv1 equals scaldiv2 = " << (scaldiv1 == scaldiv2) << std::endl;


    /** TEST NUMBER TWELVE : ISEMPTY METHOD */
    fullprint << "test number twelve : isEmpty method" << std::endl;

    /* Check method isEmpty */
    SymmetricMatrix SymmetricMatrix5;
    fullprint << "SymmetricMatrix0 is empty = " << SymmetricMatrix0.isEmpty() << std::endl
              << "SymmetricMatrix1 is empty = " << SymmetricMatrix1.isEmpty() << std::endl
              << "SymmetricMatrix5 is empty = " << SymmetricMatrix5.isEmpty() << std::endl;


    SquareMatrix L(2);
    L(0, 0) = 2.4;
    L(1, 0) = 4.0;
    L(0, 1) = 3.0;
    L(1, 1) = 1.0;

    SymmetricMatrix SM(2);
    SM(0, 0) = 1.5;
    SM(1, 0) = 1.1;
    SM(1, 1) = 3.0;

    SquareMatrix M(2);
    M(0, 0) = 1.5;
    M(1, 0) = 1.1;
    M(0, 1) = 1.1;
    M(1, 1) = 3.0;

    Matrix M2(2, 2);
    M2(0, 0) = 1.5;
    M2(1, 0) = 1.1;
    M2(0, 1) = 1.1;
    M2(1, 1) = 3.0;

    fullprint << "L * SM = " << L * SM << std::endl;
    fullprint << "L * M = " << L * M << std::endl;

    fullprint << "M2 * SM = " << M2 * SM << std::endl;
    fullprint << "M * SM = " << M * SM << std::endl;

    Matrix M3(2, 3);
    M3(0, 0) = 1.5;
    M3(1, 0) = 1.1;
    M3(0, 1) = 1.1;
    M3(1, 1) = 3.0;
    M3(0, 2) = 7.1;
    M3(1, 2) = 9.0;
    fullprint << "SM * M3 = " << SM * M3 << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}

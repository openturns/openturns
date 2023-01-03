//                                               -*- C++ -*-
/**
 *  @brief The test file of class TriangularMatrix for standard methods
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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

typedef Collection<Scalar> ScalarCollection;

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {

    /** DEFAULT CONSTRUCTOR AND STRING CONVERTER */
    fullprint << "test 0 : default constructor and string converter" << std::endl;

    /* Default constructor */
    TriangularMatrix triangularMatrix0;

    /* String converter */
    fullprint << "triangularMatrix0 = " << triangularMatrix0 << std::endl;


    /** CONSTRUCTOR WITH SIZE, OPERATOR() AND STRING CONVERTER */
    fullprint << "test 1: constructor with size, operator() and string converter" << std::endl;

    /* Constructor with size */
    TriangularMatrix triangularMatrix1(2);

    /* Check operator() methods */
    triangularMatrix1(0, 0) = 1. ;
    triangularMatrix1(1, 0) = 1. ;
    triangularMatrix1(1, 1) = 4. ;

    /* String converter */
    fullprint << "triangularMatrix1 = " << triangularMatrix1 << std::endl;
    fullprint << "is triangular lower ?  " << triangularMatrix1.isLowerTriangular() << std::endl;


    /** COPY CONSTRUCTOR AND STRING CONVERTER */
    fullprint << "test 2 : copy constructor and string converter" << std::endl;

    /* Copy constructor */
    TriangularMatrix triangularMatrix2(triangularMatrix1);

    /* String converter */
    fullprint << "triangularMatrix2 = " << triangularMatrix2 << std::endl;


    /** GET DIMENSIONS METHODS */
    fullprint << "test 3 : get dimensions methods" << std::endl;

    /* Get dimension methods */
    fullprint << "triangularMatrix1's nbRows = " << triangularMatrix1.getNbRows() << std::endl
              << "triangularMatrix1's nbColumns = " << triangularMatrix1.getNbColumns() << std::endl;


    /**  ASSIGNMENT METHOD */
    fullprint << "test 4 : assignment method" << std::endl;

    /* Assignment method */
    TriangularMatrix triangularMatrix3 ;
    triangularMatrix3 = triangularMatrix1 ;
    fullprint << "triangularMatrix3 = " << triangularMatrix3 << std::endl;

    /** TRANSPOSE AND CONJUGATE METHOD */
    fullprint << "test 5 : transpose method" << std::endl;

    /* Check transpose method */
    fullprint << "triangularMatrix1 transpose = " << triangularMatrix1.transpose() << std::endl;
    fullprint << "is transpose triangular lower ?  " << triangularMatrix1.transpose().isLowerTriangular() << std::endl;

    TriangularMatrix triangularMatrix4 = triangularMatrix1.transpose();

    /**  ADDITION METHOD */
    fullprint << "test number 6 : addition method" << std::endl;

    /* Check addition method : we check the operator and the symmetry of the operator, thus testing the comparison operator */
    SquareMatrix sum1 = triangularMatrix1.operator + ( triangularMatrix4 ) ;
    SquareMatrix sum2 = triangularMatrix4.operator + ( triangularMatrix1 ) ;
    fullprint << "sum1 = " << sum1 << std::endl;
    fullprint << "sum2 = " << sum2 << std::endl;
    fullprint << "sum1 equals sum2 = " << (sum1 == sum2) << std::endl;


    /** SUBTRACTION METHOD */
    fullprint << "test 7 : subtraction method" << std::endl;

    /* Check subtraction method */
    SquareMatrix diff = triangularMatrix1.operator - ( triangularMatrix4 ) ;
    fullprint << "diff = " << diff << std::endl;

    /**  MATRIX MULTIPLICATION METHOD */
    fullprint << "test 8 : matrix multiplication method" << std::endl;

    /* Check multiplication method */
    SquareMatrix prod = triangularMatrix1.operator * ( triangularMatrix4 ) ;
    fullprint << "prod = " << prod << std::endl;

    /**  MULTIPLICATION WITH A NUMERICAL POINT METHOD */
    fullprint << "test 9 : multiplication with a numerical point method" << std::endl;

    /* Create the numerical point */
    Point pt ;
    pt.add(1.) ;
    pt.add(2.) ;
    fullprint << "pt = " << pt << std::endl;

    /* Check the product method */
    ScalarCollection ptResult = triangularMatrix1.operator * ( pt )  ;
    fullprint << "ptResult = " << ptResult << std::endl;

    /**  MULTIPLICATION WITH A NUMERICAL COMPLEX COLLECTION METHOD */
    fullprint << "test 10 : multiplication with a numerical scalar collection method" << std::endl;

    /* Create the numerical point */
    ScalarCollection ptScalar ;
    ptScalar.add(1.) ;
    ptScalar.add(0.) ;
    fullprint << "ptScalar = " << ptScalar << std::endl;

    /* Check the product method */
    ScalarCollection ptResult2 = triangularMatrix1.operator * ( ptScalar )  ;
    fullprint << "ptResult2 = " << ptResult2 << std::endl;

    /** MULTIPLICATION AND DIVISION BY A NUMERICAL SCALAR METHODS */
    fullprint << "test 11 : multiplication and division by a numerical scalar methods" << std::endl;

    /* Check the multiplication method */
    Scalar s = 3.0;
    TriangularMatrix scalprod1 = triangularMatrix1.operator * (s) ;
    TriangularMatrix scalprod2 = s * triangularMatrix1 ;
    fullprint << "scalprod1 = " << scalprod1 << std::endl;
    fullprint << "scalprod2 = " << scalprod2 << std::endl;
    fullprint << "scalprod1 equals scalprod2 = " << (scalprod1 == scalprod2) << std::endl;

    /* Check the division method */
    TriangularMatrix scaldiv1 = triangularMatrix1.operator / (s) ;
    TriangularMatrix scaldiv2 = triangularMatrix1.operator * (1.0 / s) ;
    fullprint << "scaldiv1 = " << scaldiv1 << std::endl;
    fullprint << "scaldiv2 = " << scaldiv2 << std::endl;
    fullprint << "scaldiv1 equals scaldiv2 = " << (scaldiv1 == scaldiv2) << std::endl;


    /** ISEMPTY METHOD */
    fullprint << "test 12 : isEmpty method" << std::endl;

    /* Check method isEmpty */
    TriangularMatrix triangularMatrix5;
    fullprint << "triangularMatrix0 is empty = " << triangularMatrix0.isEmpty() << std::endl
              << "triangularMatrix1 is empty = " << triangularMatrix1.isEmpty() << std::endl
              << "triangularMatrix5 is empty = " << triangularMatrix5.isEmpty() << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}

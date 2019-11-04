//                                               -*- C++ -*-
/**
 *  @brief The test file of class HermitianMatrix for standard methods
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
    HermitianMatrix hermitianMatrix0;

    /* String converter */
    fullprint << "hermitianMatrix0 = " << hermitianMatrix0 << std::endl;


    /** CONSTRUCTOR WITH SIZE, OPERATOR() AND STRING CONVERTER */
    fullprint << "test 1: constructor with size, operator() and string converter" << std::endl;

    /* Constructor with size */
    HermitianMatrix hermitianMatrix1(2);

    /* Check operator() methods */
    hermitianMatrix1(0, 0) = 1.0 ;
    hermitianMatrix1(1, 0) = Complex(3.0, -1.0) ;
    hermitianMatrix1(1, 1) = 4.0 ;

    /* String converter */
    fullprint << "hermitianMatrix1 = " << hermitianMatrix1 << std::endl;


    /** COPY CONSTRUCTOR AND STRING CONVERTER */
    fullprint << "test 2 : copy constructor and string converter" << std::endl;

    /* Copy constructor */
    HermitianMatrix hermitianMatrix2(hermitianMatrix1);

    /* String converter */
    fullprint << "hermitianMatrix2 = " << hermitianMatrix2 << std::endl;


    /** GET DIMENSIONS METHODS */
    fullprint << "test 3 : get dimensions methods" << std::endl;

    /* Get dimension methods */
    fullprint << "hermitianMatrix1's nbRows = " << hermitianMatrix1.getNbRows() << std::endl
              << "hermitianMatrix1's nbColumns = " << hermitianMatrix1.getNbColumns() << std::endl;


    /**  ASSIGNMENT METHOD */
    fullprint << "test 4 : assignment method" << std::endl;

    /* Assignment method */
    HermitianMatrix hermitianMatrix3 ;
    hermitianMatrix3 = hermitianMatrix1 ;
    fullprint << "hermitianMatrix3 = " << hermitianMatrix3 << std::endl;

    /** CONJUGATE METHOD */
    fullprint << "test 5 : conjugate method" << std::endl;

    /* Check transpose method */
    HermitianMatrix hermitianMatrix4 = hermitianMatrix1.conjugate();
    fullprint << "hermitianMatrix1 conjugate = " << hermitianMatrix4 << std::endl;
    fullprint << "hermitianMatrix1 conjugate = " << hermitianMatrix1.conjugate() << std::endl;

    /**  ADDITION METHOD */
    fullprint << "test number 6 : addition method" << std::endl;

    /* Check addition method : we check the operator and the symmetry of the operator, thus testing the comparison operator */
    HermitianMatrix sum1 = hermitianMatrix1.operator + ( hermitianMatrix4 ) ;
    HermitianMatrix sum2 = hermitianMatrix4.operator + ( hermitianMatrix1 ) ;
    fullprint << "sum1 = " << sum1 << std::endl;
    fullprint << "sum2 = " << sum2 << std::endl;
    fullprint << "sum1 equals sum2 = " << (sum1 == sum2) << std::endl;


    /** SUBTRACTION METHOD */
    fullprint << "test 7 : subtraction method" << std::endl;

    /* Check subtraction method */
    HermitianMatrix diff1 = hermitianMatrix1.operator - ( hermitianMatrix4 ) ;
    fullprint << "diff1 = " << diff1 << std::endl;
    SquareComplexMatrix mat(2);
    mat(0, 0) = Complex(1.0, 2.0);
    mat(0, 1) = Complex(3.0, 4.0);
    mat(1, 0) = Complex(5.0, 6.0);
    mat(1, 1) = Complex(7.0, 8.0);
    SquareComplexMatrix diff2 = hermitianMatrix1.operator - ( mat ) ;
    fullprint << "diff2 = " << diff2 << std::endl;
    SquareComplexMatrix diff3 = mat.operator - (hermitianMatrix1) ;
    fullprint << "diff3 = " << diff3 << std::endl;

    /**  MATRIX MULTIPLICATION METHOD */
    fullprint << "test 8 : matrix multiplication method" << std::endl;

    /* Check multiplication method */
    SquareComplexMatrix prod = hermitianMatrix1.operator * ( hermitianMatrix4 ) ;
    fullprint << "prod = " << prod << std::endl;

    /**  MULTIPLICATION WITH A NUMERICAL POINT METHOD */
    fullprint << "test 9 : multiplication with a numerical point method" << std::endl;

    /* Create the numerical point */
    Point pt ;
    pt.add(1.) ;
    pt.add(2.) ;
    fullprint << "pt = " << pt << std::endl;

    /* Check the product method */
    ComplexCollection ptResult = hermitianMatrix1.operator * ( pt )  ;
    fullprint << "ptResult = " << ptResult << std::endl;

    /** MULTIPLICATION AND DIVISION BY A NUMERICAL SCALAR METHODS */
    fullprint << "test 10 : multiplication and division by a numerical scalar methods" << std::endl;

    /* Check the multiplication method */
    Complex s(3.0, 2.0);
    HermitianMatrix scalprod1 = hermitianMatrix1.operator * (s) ;
    HermitianMatrix scalprod2 = s * hermitianMatrix1 ;
    fullprint << "scalprod1 = " << scalprod1 << std::endl;
    fullprint << "scalprod2 = " << scalprod2 << std::endl;
    fullprint << "scalprod1 equals scalprod2 = " << (scalprod1 == scalprod2) << std::endl;

    /* Check the division method */
    HermitianMatrix scaldiv1 = hermitianMatrix1.operator / (s) ;
    HermitianMatrix scaldiv2 = hermitianMatrix1.operator * (1.0 / s) ;
    fullprint << "scaldiv1 = " << scaldiv1 << std::endl;
    fullprint << "scaldiv2 = " << scaldiv2 << std::endl;
    fullprint << "scaldiv1 equals scaldiv2 = " << (scaldiv1 == scaldiv2) << std::endl;


    /** ISEMPTY METHOD */
    fullprint << "test number twelve : isEmpty method" << std::endl;

    /* Check method isEmpty */
    HermitianMatrix hermitianMatrix5;
    fullprint << "hermitianMatrix0 is empty = " << hermitianMatrix0.isEmpty() << std::endl
              << "hermitianMatrix1 is empty = " << hermitianMatrix1.isEmpty() << std::endl
              << "hermitianMatrix5 is empty = " << hermitianMatrix5.isEmpty() << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}

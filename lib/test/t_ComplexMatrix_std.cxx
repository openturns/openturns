//                                               -*- C++ -*-
/**
 *  @brief The test file of class ComplexMatrix for standard methods
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

typedef Collection<Complex> ComplexCollection;

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  /** TEST ZERO : CHECK THE DEFAULT CONSTRUCTOR AND THE STRING CONVERTER */
  fullprint << "test 0 : default constructor and string converter" << std::endl;

  /* Default constructor */
  ComplexMatrix matrix0;

  /* String converter */
  fullprint << "matrix0 = " << matrix0 << std::endl;


  /** TEST ONE : MATRIX-CONSTRUCTOR WITH SIZE, OPERATOR() AND STRING CONVERTER */
  fullprint << "test 1 : constructor with size, operator() and string converter" << std::endl;

  /* Constructor with size */
  ComplexMatrix matrix1(2, 2);

  /* Check operator() methods */
  matrix1(0, 0) = Complex(1., 1) ;
  matrix1(1, 0) = Complex(2., -1) ;
  matrix1(0, 1) = Complex(4., 2) ;
  matrix1(1, 1) = Complex(3., 2) ;

  /* String converter */
  fullprint << "matrix1 = " << matrix1 << std::endl;


  /** TEST TWO : COPY CONSTRUCTOR */
  fullprint << "test 2 : copy constructor and string converter" << std::endl;

  /* Copy constructor */
  ComplexMatrix matrix2(matrix1);

  /* String converter */
  fullprint << "matrix2 = " << matrix2 << std::endl;


  /** TEST THREE : GET DIMENSIONS METHODS */
  fullprint << "test 3 : get dimensions methods" << std::endl;

  /* Get dimension methods */
  fullprint << "matrix1's nbRows = " << matrix1.getNbRows() << std::endl
            << "matrix1's nbColumns = " << matrix1.getNbColumns() << std::endl;


  /** TEST FOUR : CONSTRUCTOR WITH COLLECTION AND ARRAY */
  fullprint << "test 4 : constructor with collection and array method" << std::endl;

  /* Create the collection of values */
  Collection<Complex> elementsValues(6);
  elementsValues[0] = 1.;
  elementsValues[1] = 2.;
  elementsValues[2] = 3.;
  elementsValues[3] = 4.;
  elementsValues[4] = 5.;
  elementsValues[5] = 6.;

  /* Check the content of the collection */
  fullprint << "elementsValues = " << elementsValues << std::endl;

  /* Check the constructor with collection */
  ComplexMatrix matrix0bis = ComplexMatrix(2, 2, elementsValues);
  fullprint << "matrix0bis = " << matrix0bis << std::endl;

  /** TEST FIVE : ASSIGNMENT METHOD */
  fullprint << "test 5 : assignment method" << std::endl;

  /* Assignment method */
  ComplexMatrix matrix3 ;
  matrix3 = matrix1 ;
  fullprint << "matrix3 = " << matrix3 << std::endl;


  /** TEST SIX : TRANSPOSITION METHOD */
  fullprint << "test 6 : transposition method" << std::endl;

  /* Check transpose method */
  ComplexMatrix matrix4 = matrix1.transpose();
  fullprint << "matrix1 transposed = " << matrix4 << std::endl;


  /** TEST SEVEN : CONJUGATE METHOD */
  fullprint << "test 7 : conjugate method" << std::endl;

  /* Check conjugate method */
  ComplexMatrix matrix5 = matrix1.conjugate();
  fullprint << "matrix1 conjugated = " << matrix5 << std::endl;

  /** TEST EIGHT : CONJUGATE AND TRANSPOSITION METHOD */
  fullprint << "test 8 : conjugate and transposition method" << std::endl;

  /* Check conjugate method */
  ComplexMatrix matrix6 = matrix1.conjugate();
  fullprint << "matrix1 conjugated and transposed= " << matrix6 << std::endl;


  /** TEST NINE : ADDITION METHOD */


  fullprint << "test 9 : addition method" << std::endl;

  /* Check addition method : we check the operator and the symmetry of the operator, thus testing the comparison operator */
  ComplexMatrix sum1 = matrix1.operator + ( matrix4 ) ;
  ComplexMatrix sum2 = matrix4.operator + ( matrix1 ) ;
  fullprint << "sum1 = " << sum1 << std::endl;
  fullprint << "sum2 = " << sum2 << std::endl;
  fullprint << "sum1 equals sum2 = " << (sum1 == sum2) << std::endl;


  /** TEST TEN : SUBTRACTION METHOD */
  fullprint << "test 10: subtraction method" << std::endl;

  /* Check subtraction method */
  ComplexMatrix diff = matrix1.operator - ( matrix4 ) ;
  fullprint << "diff = " << diff << std::endl;


  /** TEST ELEVEN : MATRIX MULTIPLICATION METHOD */
  fullprint << "test 11 : matrix multiplication method" << std::endl;

  /* Check multiplication method */
  ComplexMatrix prod = matrix1.operator * ( matrix4 ) ;
  fullprint << "prod = " << prod << std::endl;


  /** TEST TWELVE : MULTIPLICATION WITH A NUMERICAL POINT METHOD */
  fullprint << "test 12 : multiplication with a numerical point method" << std::endl;

  /* Create the numerical point */
  Point pt ;
  pt.add(1.) ;
  pt.add(2.) ;
  fullprint << "pt = " << pt << std::endl;

  /* Check the product method */
  ComplexCollection ptResult = matrix1.operator * ( pt ) ;
  fullprint << "ptResult = " << ptResult << std::endl;

  /** TEST THIRTEEN : MULTIPLICATION AND DIVISION BY A NUMERICAL SCALAR METHODS */
  fullprint << "test 13 : multiplication and division by a complex methods" << std::endl;

  /* Check the multiplication method */
  Complex s(3., 1.0);
  ComplexMatrix scalprod1 = matrix1.operator * (s) ;
  ComplexMatrix scalprod2 = s * matrix1 ;
  ComplexMatrix scalprod3 = matrix1 * s ;
  fullprint << "scalprod1 = " << scalprod1 << std::endl;
  fullprint << "scalprod2 = " << scalprod2 << std::endl;
  fullprint << "scalprod3 = " << scalprod3 << std::endl;
  fullprint << "scalprod1 equals scalprod2 = " << (scalprod1 == scalprod2) << std::endl;
  fullprint << "scalprod1 equals scalprod3 = " << (scalprod1 == scalprod3) << std::endl;
  fullprint << "scalprod2 equals scalprod3 = " << (scalprod2 == scalprod3) << std::endl;

  /* Check the division method */
  ComplexMatrix scaldiv1 = matrix1.operator / (s) ;
  ComplexMatrix scaldiv2 = matrix1.operator / (s) ;
  fullprint << "scaldiv1 = " << scaldiv1 << std::endl;
  fullprint << "scaldiv2 = " << scaldiv2 << std::endl;
  fullprint << "scaldiv1 equals scaldiv2 = " << (scaldiv1 == scaldiv2) << std::endl;


  /** TEST FOURTEEN : ISEMPTY METHOD */
  fullprint << "test 14 : isEmpty method" << std::endl;

  /* Check method isEmpty */
  ComplexMatrix matrix7;
  ComplexMatrix matrix8;
  //  matrix6.setDimensions(0,3);
  fullprint << "matrix1 is empty = " << matrix1.isEmpty() << std::endl
            << "matrix7 is empty = " << matrix7.isEmpty() << std::endl
            << "matrix8 is empty = " << matrix8.isEmpty() << std::endl
            << "matrix0 is empty = " << matrix0.isEmpty() << std::endl;

  /** TEST FIFTEEN : MULTIPLICATION WITH A NUMERICAL POINT METHOD */
  fullprint << "test 15 : multiplication with a numerical point method" << std::endl;

  /* Create the numerical point */
  Point pt_test ;
  pt_test.add(1.) ;
  pt_test.add(2.) ;
  fullprint << "pt_test = " << pt_test << std::endl;

  ComplexMatrix A(2, 2);
  A(0, 0) = 0.5 ;
  A(1, 0) = -(std::sqrt(3.) / 2) ;
  A(0, 1) = (std::sqrt(3.) / 2) ;
  A(1, 1) = 0.5 ;
  ComplexMatrix B = A.transpose();
  ComplexMatrix id = B.operator * (A);

  /* Check the product method */
  ComplexCollection ptResult2 = id.operator * ( pt_test ) ;
  fullprint << "A = " << A << std::endl;
  fullprint << "B = " << B << std::endl;
  fullprint << "id = " << id << std::endl;
  fullprint << "ptResult2 = " << ptResult2 << std::endl;


  return ExitCode::Success;
}

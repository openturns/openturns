//                                               -*- C++ -*-
/**
 *  @brief The test file of class Matrix for standard methods
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

  /** TEST NUMBER ZERO : DEFAULT CONSTRUCTOR AND STRING CONVERTER */
  fullprint << "test number zero : default constructor and string converter" << std::endl;

  /* Default constructor */
  Matrix matrix0;

  /* String converter */
  fullprint << "matrix0 = " << matrix0 << std::endl;


  /** TEST NUMBER ONE : CONSTRUCTOR WITH SIZE, OPERATOR() AND STRING CONVERTER */
  fullprint << "test number one : constructor with size, operator() and string converter" << std::endl;

  /* Constructor with size */
  Matrix matrix1(2, 2);

  /* Check operator() methods */
  matrix1(0, 0) = 1. ;
  matrix1(1, 0) = 2. ;
  matrix1(0, 1) = 3. ;
  matrix1(1, 1) = 4. ;

  /* String converter */
  fullprint << "matrix1 = " << matrix1 << std::endl;


  /** TEST NUMBER TWO : COPY CONSTRUCTOR AND STRING CONVERTER */
  fullprint << "test number two : copy constructor and string converter" << std::endl;

  /* Copy constructor */
  Matrix matrix2(matrix1);

  /* String converter */
  fullprint << "matrix2 = " << matrix2 << std::endl;


  /** TEST NUMBER THREE : GET DIMENSIONS METHODS */
  fullprint << "test number three : get dimensions methods" << std::endl;

  /* Get dimension methods */
  fullprint << "matrix1's nbRows = " << matrix1.getNbRows() << std::endl
            << "matrix1's nbColumns = " << matrix1.getNbColumns() << std::endl;


  /** TEST NUMBER FOUR : CONSTRUCTOR WITH COLLECTION AND ARRAY */
  fullprint << "test number four : constructor with collection and array method" << std::endl;

  /* Create the collection of values */
  Collection<double> elementsValues;
  elementsValues.add(1.);
  elementsValues.add(2.);
  elementsValues.add(3.);
  elementsValues.add(4.);
  elementsValues.add(5.);
  elementsValues.add(6.);

  /* Check the content of the collection */
  fullprint << "elementsValues = " << elementsValues << std::endl;

  /* Check the constructor with collection */
  Matrix matrix0bis = Matrix(2, 2, elementsValues);
  fullprint << "matrix0bis = " << matrix0bis << std::endl;

#if 0
  /* Create the array */
  double tab[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};

  /* Check the content of the array */
  fullprint << tab[0] << ';' << tab[1] << ';' << tab[2] << ';' << tab[3] << ';' << tab[4] << ';' << tab[5] << ';' << std::endl;
  /* Check the constructor with array */
  Matrix matrix0ter = Matrix(4, 4, tab, tab + 6);
  fullprint << "matrix0ter = " << matrix0ter << std::endl;
#endif
  /** TEST NUMBER FIVE : ASSIGNMENT METHOD */
  fullprint << "test number five : assignment method" << std::endl;

  /* Assignment method */
  Matrix matrix3 ;
  matrix3 = matrix1 ;
  fullprint << "matrix3 = " << matrix3 << std::endl;



  /** TEST NUMBER SIX : TRANSPOSITION METHOD */
  fullprint << "test number six : transposition method" << std::endl;

  /* Check transpose method */
  Matrix matrix4 = matrix1.transpose();
  fullprint << "matrix1 transposed = " << matrix4 << std::endl;


  /** TEST NUMBER SEVEN : ADDITION METHOD */
  fullprint << "test number seven : addition method" << std::endl;

  /* Check addition method : we check the operator and the symmetry of the operator, thus testing the comparison operator */
  Matrix sum1 = matrix1.operator + ( matrix4 ) ;
  Matrix sum2 = matrix4.operator + ( matrix1 ) ;
  fullprint << "sum1 = " << sum1 << std::endl;
  fullprint << "sum2 = " << sum2 << std::endl;
  fullprint << "sum1 equals sum2 = " << (sum1 == sum2) << std::endl;


  /** TEST NUMBER EIGHT : SUBTRACTION METHOD */
  fullprint << "test number eight : subtraction method" << std::endl;

  /* Check subtraction method */
  Matrix diff = matrix1.operator - ( matrix4 ) ;
  fullprint << "diff = " << diff << std::endl;


  /** TEST NUMBER NINE : MATRIX MULTIPLICATION METHOD */
  fullprint << "test number nine : matrix multiplication method" << std::endl;

  /* Check multiplication method */
  Matrix prod = matrix1.operator * ( matrix4 ) ;
  fullprint << "prod = " << prod << std::endl;


  /** TEST NUMBER TEN : MULTIPLICATION WITH A POINT METHOD */
  fullprint << "test number ten : multiplication with a point method" << std::endl;

  /* Create the point */
  Point pt ;
  pt.add(1.) ;
  pt.add(2.) ;
  fullprint << "pt = " << pt << std::endl;

  /* Check the product method */
  Point ptResult = matrix1.operator * ( pt ) ;
  fullprint << "ptResult = " << ptResult << std::endl;

  /** TEST NUMBER ELEVEN : MULTIPLICATION AND DIVISION BY A SCALAR METHODS */
  fullprint << "test number eleven : multiplication and division by a scalar methods" << std::endl;

  /* Check the multiplication method */
  double s = 3.;
  Matrix scalprod1 = matrix1.operator * (s) ;
  Matrix scalprod2 = s * matrix1 ;
  Matrix scalprod3 = matrix1 * s ;
  fullprint << "scalprod1 = " << scalprod1 << std::endl;
  fullprint << "scalprod2 = " << scalprod2 << std::endl;
  fullprint << "scalprod3 = " << scalprod3 << std::endl;
  fullprint << "scalprod1 equals scalprod2 = " << (scalprod1 == scalprod2) << std::endl;
  fullprint << "scalprod1 equals scalprod3 = " << (scalprod1 == scalprod3) << std::endl;
  fullprint << "scalprod2 equals scalprod3 = " << (scalprod2 == scalprod3) << std::endl;

  /* Check the division method */
  Matrix scaldiv1 = matrix1.operator / (s) ;
  Matrix scaldiv2 = matrix1.operator * (1 / s) ;
  fullprint << "scaldiv1 = " << scaldiv1 << std::endl;
  fullprint << "scaldiv2 = " << scaldiv2 << std::endl;
  fullprint << "scaldiv1 equals scaldiv2 = " << (scaldiv1 == scaldiv2) << std::endl;


  /** TEST NUMBER TWELVE : ISEMPTY METHOD */
  fullprint << "test number twelve : isEmpty method" << std::endl;

  /* Check method isEmpty */
  Matrix matrix5;
  Matrix matrix6;
  //  matrix6.setDimensions(0,3);
  fullprint << "matrix1 is empty = " << matrix1.isEmpty() << std::endl
            << "matrix5 is empty = " << matrix5.isEmpty() << std::endl
            << "matrix6 is empty = " << matrix6.isEmpty() << std::endl
            << "matrix0 is empty = " << matrix0.isEmpty() << std::endl;

  /** TEST NUMBER FOURTEEN : MULTIPLICATION WITH A POINT METHOD */
  fullprint << "test number fourteen : multiplication with a point method" << std::endl;

  /* Create the point */
  Point pt_test ;
  pt_test.add(1.) ;
  pt_test.add(2.) ;
  fullprint << "pt_test = " << pt_test << std::endl;

  Matrix A(2, 2);
  A(0, 0) = 0.5 ;
  A(1, 0) = -(std::sqrt(3.) / 2) ;
  A(0, 1) = (std::sqrt(3.) / 2) ;
  A(1, 1) = 0.5 ;
  Matrix B = A.transpose();
  Matrix id = B.operator * (A);

  /* Check the product method */
  Point ptResult2 = id.operator * ( pt_test ) ;
  fullprint << "A = " << A << std::endl;
  fullprint << "B = " << B << std::endl;
  fullprint << "id = " << id << std::endl;
  fullprint << "ptResult2 = " << ptResult2 << std::endl;


  /** TEST NUMBER FIFTEEN : MULTIPLICATION WITH A SAMPLE METHOD */
  fullprint << "test number fifteen : multiplication with a sample method" << std::endl;
  Sample S(2, 4);
  S(0, 0) = 1.0;
  S(0, 1) = 3.0;
  S(0, 2) = -1.0;
  S(0, 3) = -3.0;
  S(1, 0) = -2.0;
  S(1, 1) = -5.0;
  S(1, 2) = 3.0;
  S(1, 3) = 1.0;
  Matrix matrix32 = Matrix(3, 2, elementsValues);
  Matrix matrix23 = Matrix(2, 3, elementsValues);
  Collection<double> elementsValues12(elementsValues);
  for(UnsignedInteger i = 7; i < 13; ++i)
    elementsValues12.add(i);
  Matrix matrix34 = Matrix(3, 4, elementsValues12);
  Matrix matrix43 = Matrix(4, 3, elementsValues12);
  fullprint << "matrix32 = " << matrix32 << std::endl;
  fullprint << "matrix23 = " << matrix23 << std::endl;
  fullprint << "matrix34 = " << matrix34 << std::endl;
  fullprint << "matrix43 = " << matrix43 << std::endl;
  fullprint << "S = " << S << std::endl;
  Sample resultS = matrix32.operator * (S);
  fullprint << "matrix32*S = " << resultS << std::endl;
  Sample resultS1(matrix32.getImplementation()->genSampleProd(S, false, false, 'L'));
  fullprint << "matrix32*S = " << resultS1 << std::endl;
  Sample resultS2(matrix23.getImplementation()->genSampleProd(S, true, false, 'L'));
  fullprint << "matrix23^T*S = " << resultS2 << std::endl;
  Sample resultS3(matrix43.getImplementation()->genSampleProd(S, false, false, 'R'));
  fullprint << "S*matrix43 = " << resultS3 << std::endl;
  Sample resultS4(matrix34.getImplementation()->genSampleProd(S, true, false, 'R'));
  fullprint << "S*matrix34^T = " << resultS4 << std::endl;
  Sample S2(4, 2);
  S2(0, 0) = S(0, 0);
  S2(1, 0) = S(0, 1);
  S2(2, 0) = S(0, 2);
  S2(3, 0) = S(0, 3);
  S2(0, 1) = S(1, 0);
  S2(1, 1) = S(1, 1);
  S2(2, 1) = S(1, 2);
  S2(3, 1) = S(1, 3);
  fullprint << "S2 = " << S2 << std::endl;
  Sample resultS1T(matrix32.getImplementation()->genSampleProd(S2, false, true, 'L'));
  fullprint << "matrix32*S2^T = " << resultS1T << std::endl;
  Sample resultS2T(matrix23.getImplementation()->genSampleProd(S2, true, true, 'L'));
  fullprint << "matrix23^T*S2^T = " << resultS2T << std::endl;
  Sample resultS3T(matrix43.getImplementation()->genSampleProd(S2, false, true, 'R'));
  fullprint << "S2^T*matrix43 = " << resultS3T << std::endl;
  Sample resultS4T(matrix34.getImplementation()->genSampleProd(S2, true, true, 'R'));
  fullprint << "S2^T*matrix34^T = " << resultS4T << std::endl;

  // resize
  Collection<Matrix> coll;
  Matrix A1(2, 2);
  A1(0, 0) = 1.0;
  A1(1, 0) = 2.0;
  A1(0, 1) = 3.0;
  A1(1, 1) = 4.0;
  coll.add(A1);
  Matrix A2(3, 2);
  A2(0, 0) = 1.0;
  A2(1, 0) = 2.0;
  A2(2, 0) = 3.0;
  A2(0, 1) = 4.0;
  A2(1, 1) = 5.0;
  A2(2, 1) = 6.0;
  coll.add(A2);
  Matrix A3(2, 3);
  A3(0, 0) = 1.0;
  A3(1, 0) = 2.0;
  A3(0, 1) = 3.0;
  A3(1, 1) = 4.0;
  A3(0, 2) = 5.0;
  A3(1, 2) = 6.0;
  coll.add(A3);
  for (UnsignedInteger i = 0; i < 3; ++ i)
  {
    const Matrix A(coll[i]);
    fullprint << "A = " << A.__str__() << std::endl;
    for(UnsignedInteger newRow = 1; newRow < 6; ++ newRow)
    {
      for(UnsignedInteger newCol = 1; newCol < 6; ++ newCol)
      {
        MatrixImplementation B(*A.getImplementation());
        fullprint << "----------" << newRow << "x" << newCol <<std::endl;
        B.resize(newRow, newCol);
        fullprint << B.__str__() << std::endl;
      }
    }
  }
  return ExitCode::Success;
}

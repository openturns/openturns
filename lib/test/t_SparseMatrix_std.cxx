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
  SparseMatrix matrix0;

  /* String converter */
  fullprint << "matrix0 = " << matrix0 << std::endl;


  /** TEST NUMBER ONE : CONSTRUCTOR WITH SIZE, OPERATOR() AND STRING CONVERTER */
  fullprint << "test number one : constructor with size, operator() and string converter" << std::endl;

  /* Constructor with size */
  SparseMatrix matrix1(4, 5);

  /* Check operator() methods */
  matrix1(0, 2) = 5. ;
  matrix1(1, 0) = 8. ;
  matrix1(2, 4) = 2. ;
  matrix1(3, 2) = 9. ;

  /* String converter */
  fullprint << "matrix1 = " << matrix1 << std::endl;
  fullprint << "matrix1 as dense = " << matrix1.asDenseMatrix().__str__() << std::endl;

  /** TEST NUMBER TWO : COPY CONSTRUCTOR AND STRING CONVERTER */
  fullprint << "test number two : copy constructor and string converter" << std::endl;

  /* Copy constructor */
  SparseMatrix matrix2(matrix1);

  /* String converter */
  fullprint << "matrix2 = " << matrix2 << std::endl;


  /** TEST NUMBER THREE : GET DIMENSIONS METHODS */
  fullprint << "test number three : get dimensions methods" << std::endl;

  /* Get dimension methods */
  fullprint << "matrix1's nbRows = " << matrix1.getNbRows() << std::endl
            << "matrix1's nbColumns = " << matrix1.getNbColumns() << std::endl
            << "matrix1's nbNonZeros = " << matrix1.getNbNonZeros() << std::endl;


  /** TEST NUMBER FOUR : TRANSPOSITION METHOD */
  fullprint << "test number six : transposition method" << std::endl;

  /* Check transpose method */
  SparseMatrix matrix4 = matrix1.transpose();
  fullprint << "matrix1 transposed = " << matrix4.asDenseMatrix().__str__() << std::endl;


  /* Create the point */
  Point pt(5);
  for(UnsignedInteger i = 0; i < 5; ++ i)
    pt[i] = i + 1;
  fullprint << "pt = " << pt << std::endl;

  // mul with Point
  Point ptResult = matrix1.operator * ( pt ) ;
  fullprint << "ptResult = " << ptResult << std::endl;

  // multiply with Matrix
  Matrix dense1(5, 6);
  for(UnsignedInteger i = 0; i < dense1.getNbRows(); ++ i)
    for(UnsignedInteger j = 0; j < dense1.getNbColumns(); ++ j)
      dense1(i, j) = i + j * 10;

  fullprint << "dense1=" << dense1.__str__() << std::endl;
  Matrix res2(matrix1 * dense1);
  fullprint << "sparse*dense=" << res2.__str__() << std::endl;
  fullprint << "same=" << (res2 == matrix1.asDenseMatrix() * dense1) << std::endl;

  /** triplet ctor */
  Indices rowIndices;
  Indices columnIndices;
  Point values;

  rowIndices.add(0);
  columnIndices.add(2);
  values.add(5.0);

  rowIndices.add(1);
  columnIndices.add(0);
  values.add(8.0);

  rowIndices.add(2);
  columnIndices.add(4);
  values.add(2.0);

  rowIndices.add(3);
  columnIndices.add(2);
  values.add(9.0);

  SparseMatrix matrix3(4, 5, rowIndices, columnIndices, values);
  fullprint << "matrix3=" << matrix3 << std::endl;
  fullprint << "matrix3 as dense = " << matrix3.asDenseMatrix().__str__() << std::endl;
  fullprint << "ptResult = " << matrix3*pt << std::endl;

  // item accessor
  Scalar a24 = matrix3(2, 4);
  fullprint << "matrix3(2, 4)=" << a24 << std::endl;
  matrix3(2, 4) = 8.0;
  a24 = matrix3(2, 4);
  fullprint << "matrix3(2, 4)=" << a24 << std::endl;

  // sum values at duplicate indices
  rowIndices.add(2);
  columnIndices.add(4);
  values.add(2.0);

  SparseMatrix matrix5(4, 5, rowIndices, columnIndices, values);
  fullprint << "matrix5=" << matrix5 << std::endl;
  fullprint << "matrix5 as dense = " << matrix5.asDenseMatrix().__str__() << std::endl;
  fullprint << "ptResult = " << matrix5*pt << std::endl;

  return ExitCode::Success;
}

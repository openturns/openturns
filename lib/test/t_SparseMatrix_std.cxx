//                                               -*- C++ -*-
/**
 *  @brief The test file of class Matrix for standard methods
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
  SparseMatrix matrix1(10, 10);

  /* Check operator() methods */
  matrix1(0, 0) = 1. ;
  matrix1(5, 0) = 2. ;
  matrix1(0, 2) = 3. ;
  matrix1(3, 4) = 4. ;

  /* String converter */
  fullprint << "matrix1 = " << matrix1 << std::endl;


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
  fullprint << "matrix1 transposed = " << matrix4 << std::endl;


  /** TEST NUMBER FIVE : MULTIPLICATION WITH A POINT METHOD */
  fullprint << "test number ten : multiplication with a point method" << std::endl;

  /* Create the point */
  Point pt(10) ;
  pt[0] = 1;
  pt[1] = 0;
  pt[2] = 3;
  pt[3] = 4;
  pt[4] = -5;
  pt[5] = 6.7;
  pt[6] = 0;
  pt[7] = 8;
  pt[8] = 9.2;
  pt[9] = 10;
  fullprint << "pt = " << pt << std::endl;

  /* Check the product method */
  Point ptResult = matrix1.operator * ( pt ) ;
  fullprint << "ptResult = " << ptResult << std::endl;

  return ExitCode::Success;
}

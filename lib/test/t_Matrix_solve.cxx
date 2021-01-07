//                                               -*- C++ -*-
/**
 *  @brief The test file for the solveLinearSystem method of the Matrix class
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
  // First an n by n case
  Matrix matrix1(2, 2);
  matrix1.setName("matrix1");
  matrix1(0, 0) =  1.0;
  matrix1(1, 0) =  2.0;
  matrix1(0, 1) =  5.0;
  matrix1(1, 1) = 12.0;
  fullprint << "matrix1=" << matrix1 << std::endl;

  Point pt1 ;
  pt1.add(5.0) ;
  pt1.add(0.0) ;
  fullprint << "pt1=" << pt1 << std::endl;

  Point result1 ;
  result1 = matrix1.solveLinearSystem(pt1);
  fullprint << "result1=" << result1 << std::endl;
  // Second an n by p case, n < p
  Matrix matrix2(2, 3);
  matrix2.setName("matrix2");
  matrix2(0, 0) =  1.0;
  matrix2(1, 0) =  2.0;
  matrix2(0, 1) =  5.0;
  matrix2(1, 1) = 12.0;
  matrix2(0, 2) =  3.0;
  matrix2(1, 2) =  4.0;
  fullprint << "matrix2=" << matrix2 << std::endl;

  Point pt2 ;
  pt2.add(5.0);
  pt2.add(0.0);
  fullprint << "pt2=" << pt2 << std::endl;

  Point result2 ;
  result2 = matrix2.solveLinearSystem(pt2);
  fullprint << "result2=" << result2 << std::endl;

  // Third an n by p case, n > p
  Matrix matrix3(3, 2);
  matrix3.setName("matrix3");
  matrix3(0, 0) =  1.0;
  matrix3(1, 0) =  2.0;
  matrix3(2, 0) =  4.0;
  matrix3(0, 1) =  5.0;
  matrix3(1, 1) = 12.0;
  matrix3(2, 1) =  3.0;

  fullprint << "matrix3=" << matrix3 << std::endl;

  Point pt3 ;
  pt3.add(5.0) ;
  pt3.add(0.0) ;
  pt3.add(1.0) ;
  fullprint << "pt3=" << pt3 << std::endl;

  Point result3 ;
  result3 = matrix3.solveLinearSystem(pt3);
  fullprint << "result3=" << result3 << std::endl;

  Matrix b1(2, 4);
  b1(0, 0) =  5.0;
  b1(1, 0) =  1.0;
  b1(0, 1) = 10.0;
  b1(1, 1) =  2.0;
  b1(0, 2) = 15.0;
  b1(1, 2) =  3.0;
  b1(0, 3) = 20.0;
  b1(1, 3) =  4.0;
  fullprint << "b1=" << b1 << std::endl;
  Matrix result4;
  result4 = matrix1.solveLinearSystem(b1);
  fullprint << "result4=" << result4 << std::endl;

  Matrix result5;
  result5 = matrix2.solveLinearSystem(b1);
  fullprint << "result5=" << result5 << std::endl;

  Matrix b2(3, 4);
  b2(0, 0) =  5.0;
  b2(1, 0) =  1.0;
  b2(2, 0) = -2.0;
  b2(0, 1) = 10.0;
  b2(1, 1) =  2.0;
  b2(2, 1) = -4.0;
  b2(0, 2) = 15.0;
  b2(1, 2) =  3.0;
  b2(2, 2) = -6.0;
  b2(0, 3) = 20.0;
  b2(1, 3) =  4.0;
  b2(2, 3) = -8.0;
  fullprint << "b2=" << b2 << std::endl;

  Matrix result6;
  result6 = matrix3.solveLinearSystem(b2);
  fullprint << "result6=" << result6 << std::endl;

  return ExitCode::Success;
}

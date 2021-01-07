//                                               -*- C++ -*-
/**
 *  @brief The test file for the IdentityMatrix class
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

  IdentityMatrix matrix1(2);
  matrix1.setName("matrix1");
  fullprint << "matrix1 = " << matrix1 << std::endl;

  Point pt ;
  pt.add(5.) ;
  pt.add(0.) ;
  fullprint << "pt = " << pt << std::endl;

  Point result ;
  result = matrix1.solveLinearSystem(pt);
  fullprint << "result = " << result << std::endl;

  Scalar determinant ;
  determinant = matrix1.computeDeterminant();
  fullprint << "determinant = " << determinant << std::endl;

  Collection<Scalar> ev(2);
  ev = matrix1.computeEigenValues().getCollection();
  fullprint << "ev = " << ev << std::endl;

  bool isSPD = matrix1.isPositiveDefinite();
  fullprint << "isSPD = " << isSPD << std::endl;

  SquareMatrix matrix2 = matrix1.computeCholesky();
  fullprint << "matrix2 = " << matrix2 << std::endl;

  return ExitCode::Success;
}

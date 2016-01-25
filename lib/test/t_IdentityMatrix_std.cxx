//                                               -*- C++ -*-
/**
 *  @brief The test file for the IdentityMatrix class
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "OT.hxx"
#include "OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  IdentityMatrix matrix1(2);
  matrix1.setName("matrix1");
  fullprint << "matrix1 = " << matrix1 << std::endl;

  NumericalPoint pt ;
  pt.add(5.) ;
  pt.add(0.) ;
  fullprint << "pt = " << pt << std::endl;

  NumericalPoint result ;
  result = matrix1.solveLinearSystem(pt);
  fullprint << "result = " << result << std::endl;

  NumericalScalar determinant ;
  determinant = matrix1.computeDeterminant();
  fullprint << "determinant = " << determinant << std::endl;

  Collection<NumericalScalar> ev(2);
  ev = matrix1.computeEigenValues().getCollection();
  fullprint << "ev = " << ev << std::endl;

  bool isSPD = matrix1.isPositiveDefinite();
  fullprint << "isSPD = " << isSPD << std::endl;

  SquareMatrix matrix2 = matrix1.computeCholesky();
  fullprint << "matrix2 = " << matrix2 << std::endl;

  return ExitCode::Success;
}

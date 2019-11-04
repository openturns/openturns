//                                               -*- C++ -*-
/**
 *  @brief The test file for the Lapack based methods of the SquareMatrix class
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

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {
    SymmetricMatrix matrix1(2);
    matrix1.setName("matrix1");
    matrix1(0, 0) =  1.0;
    matrix1(1, 0) =  5.0;
    matrix1(1, 1) = 12.0;
    fullprint << "matrix1=" << matrix1 << std::endl;
    fullprint << "matrix1=\n" << matrix1.__str__() << std::endl;

    Point pt ;
    pt.add(5.0);
    pt.add(0.0);
    fullprint << "pt=" << pt << std::endl;

    Point result ;
    result = matrix1.solveLinearSystem(pt);
    fullprint << "result=" << result << std::endl;
    //    fullprint << "verif. " << matrix1 * result - pt << std::endl;

    Scalar determinant ;
    determinant = matrix1.computeDeterminant();
    fullprint << "determinant=" << determinant << std::endl;

    Matrix b(2, 3);
    b(0, 0) =  5.0;
    b(1, 0) =  0.0;
    b(0, 1) = 10.0;
    b(1, 1) =  1.0;
    b(0, 2) = 15.0;
    b(1, 2) =  2.0;
    Matrix result2 ;
    result2 = matrix1.solveLinearSystem(b);
    fullprint << "result2=" << result2 << std::endl;
    fullprint << "result2=\n" << result2.__str__() << std::endl;

    Collection<Scalar> ev(2);
    ev = matrix1.computeEigenValues();
    fullprint << "ev=" << ev << std::endl;

    SquareMatrix evect(2);
    ev = matrix1.computeEV(evect);
    fullprint << "ev=" << ev << std::endl;
    fullprint << "evect=" << evect << std::endl;
    fullprint << "evect=\n" << evect.__str__() << std::endl;

    // Check the high dimension determinant computation
    SymmetricMatrix matrix3(3);
    matrix3(0, 0) = 1.0;
    matrix3(0, 1) = 2.0;
    matrix3(0, 2) = 3.0;
    matrix3(1, 1) = 2.5;
    matrix3(1, 2) = -3.5;
    matrix3(2, 2) = 2.5;

    fullprint << "matrix3=\n" << matrix3.__str__() << std::endl;
    Scalar sign;
    Scalar value = matrix3.computeLogAbsoluteDeterminant(sign);
    fullprint << "log(|det|)=" << value << ", sign=" << sign << std::endl;
    value = matrix3.computeDeterminant();
    fullprint << "det=" << value << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}

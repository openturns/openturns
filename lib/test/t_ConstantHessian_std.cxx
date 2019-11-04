//                                               -*- C++ -*-
/**
 *  @brief The test file of class ConstantHessian for standard methods
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
    PlatformInfo::SetNumericalPrecision(4);
    UnsignedInteger inputDimension = 3;
    UnsignedInteger outputDimension = 2;
    // Constant term
    SymmetricTensor constant(inputDimension, outputDimension);
    constant(0, 0, 0) = 7.0;
    constant(0, 0, 1) = -7.0;
    constant(0, 1, 0) = 8.0;
    constant(0, 1, 1) = -8.0;
    constant(0, 2, 0) = 9.0;
    constant(0, 2, 1) = -9.0;
    constant(1, 0, 0) = 8.0;
    constant(1, 0, 1) = -8.0;
    constant(1, 1, 0) = 10.0;
    constant(1, 1, 1) = -10.0;
    constant(1, 2, 0) = 11.0;
    constant(1, 2, 1) = -11.0;
    constant(2, 0, 0) = 9.0;
    constant(2, 0, 1) = -9.0;
    constant(2, 1, 0) = 11.0;
    constant(2, 1, 1) = -11.0;
    constant(2, 2, 0) = 12.0;
    constant(2, 2, 1) = -12.0;

    ConstantHessian myHessian(constant);
    myHessian.setName("constantHessian");
    Point inPoint(inputDimension);
    inPoint[0] = 7.0;
    inPoint[1] = 8.0;
    inPoint[2] = 9.0;
    SymmetricTensor outTensor = myHessian.hessian( inPoint );
    fullprint << "myHessian=" << myHessian << std::endl;
    fullprint << myHessian.getName() << "( " << inPoint << " ) = " << outTensor << std::endl;
    fullprint << myHessian.getName() << "( " << inPoint << " ) = " << outTensor.__str__() << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}

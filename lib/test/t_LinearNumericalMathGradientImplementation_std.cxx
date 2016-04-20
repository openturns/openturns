//                                               -*- C++ -*-
/**
 *  @brief The test file of class LinearNumericalMathGradientImplementation for standard methods
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
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {
    UnsignedInteger inputDimension = 3;
    UnsignedInteger outputDimension = 2;
    // Center
    NumericalPoint center(inputDimension);
    center[0] = -1.0;
    center[1] = 0.5;
    center[2] = 1.0;
    // Constant term
    Matrix constant(inputDimension, outputDimension);
    constant(0, 0) = 1.0;
    constant(1, 0) = 2.0;
    constant(2, 0) = 5.0;
    constant(0, 1) = 7.0;
    constant(1, 1) = 9.0;
    constant(2, 1) = 3.0;
    // Linear term
    SymmetricTensor linear(inputDimension, outputDimension);
    linear(0, 0, 0) = 7.0;
    linear(0, 0, 1) = -7.0;
    linear(0, 1, 0) = 8.0;
    linear(0, 1, 1) = -8.0;
    linear(0, 2, 0) = 9.0;
    linear(0, 2, 1) = -9.0;
    linear(1, 0, 0) = 8.0;
    linear(1, 0, 1) = -8.0;
    linear(1, 1, 0) = 10.0;
    linear(1, 1, 1) = -10.0;
    linear(1, 2, 0) = 11.0;
    linear(1, 2, 1) = -11.0;
    linear(2, 0, 0) = 9.0;
    linear(2, 0, 1) = -9.0;
    linear(2, 1, 0) = 11.0;
    linear(2, 1, 1) = -11.0;
    linear(2, 2, 0) = 12.0;
    linear(2, 2, 1) = -12.0;

    LinearNumericalMathGradientImplementation myGradient(center, constant, linear);
    myGradient.setName("linearGradient");
    NumericalPoint inPoint(inputDimension);
    inPoint[0] = 7.0;
    inPoint[1] = 8.0;
    inPoint[2] = 9.0;
    Matrix outMatrix = myGradient.gradient( inPoint );
    fullprint << "myGradient=" << myGradient << std::endl;
    fullprint << myGradient.getName() << "( " << inPoint << " ) = " << outMatrix << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}

//                                               -*- C++ -*-
/**
 *  @brief The test file of class CenteredFiniteDifferenceGradient for standard methods
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
    NumericalScalar eps = 1e-2;
    /** Instance creation */
    Description input(2);
    input[0] = "x1";
    input[1] = "x2";
    Description output(3);
    output[0] = "f1";
    output[1] = "f2";
    output[2] = "f3";
    Description formula(3);
    formula[0] = "x1*sin(x2)";
    formula[1] = "cos(x1+x2)";
    formula[2] = "(x2+1)*exp(x1-2*x2)";
    NumericalMathFunction myFunc(input, output, formula);
    NumericalPoint epsilon(myFunc.getInputDimension(), eps);
    NumericalPoint in(epsilon.getDimension(), 1.0);
    CenteredFiniteDifferenceGradient myGradient(epsilon, myFunc.getEvaluation());

    fullprint << "myGradient=" << myGradient << std::endl;
    fullprint << "myFunc.gradient(" << in << ")=" << myFunc.gradient(in) << std::endl;
    fullprint << "myGradient.gradient(" << in << ")=" << myGradient.gradient(in) << std::endl;
    /** Substitute the gradient */
    myFunc.setGradient(new CenteredFiniteDifferenceGradient(myGradient));
    fullprint << "myFunc.gradient(" << in << ")=" << myFunc.gradient(in) << " (after substitution)" << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}

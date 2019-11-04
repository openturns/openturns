//                                               -*- C++ -*-
/**
 *  @brief The test file of class NonCenteredFiniteDifferenceGradient for standard methods
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
    Scalar eps = 1e-4;
    /** Instance creation */
    Description input(2);
    input[0] = "x1";
    input[1] = "x2";
    Description formula(3);
    formula[0] = "x1*sin(x2)";
    formula[1] = "cos(x1+x2)";
    formula[2] = "(x2+1)*exp(x1-2*x2)";
    SymbolicFunction myFunc(input, formula);
    Point epsilon(myFunc.getInputDimension(), eps);
    Point in(epsilon.getDimension(), 1.0);
    NonCenteredFiniteDifferenceGradient myGradient(epsilon, myFunc.getEvaluation());

    fullprint << "myGradient=" << myGradient << std::endl;
    fullprint << "myFunc.gradient(" << in << ")=" << myFunc.gradient(in).__repr__() << std::endl;
    fullprint << "myGradient.gradient(" << in << ")=" << myGradient.gradient(in).__repr__() << std::endl;
    /** Substitute the gradient */
    myFunc.setGradient(new NonCenteredFiniteDifferenceGradient(myGradient));
    fullprint << "myFunc.gradient(" << in << ")=" << myFunc.gradient(in).__repr__() << " (after substitution)" << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}

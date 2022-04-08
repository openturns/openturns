//                                               -*- C++ -*-
/**
 *  @brief The test file of class Function for standard methods
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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

    // Test basic functionnalities
    //checkClassWithClassName<ComputedEvaluationImplementation>();

    /** Instance creation */
    Description input(2);
    input[0] = "x1";
    input[1] = "x2";
    Description formula(3);
    formula[0] = "x1*sin(x2)";
    formula[1] = "cos(x1+x2)";
    formula[2] = "(x2+1)*exp(x1-2*x2)";
    SymbolicFunction myFunc(input, formula);

    /** Copy constructor */
    Function newFunc(myFunc);

    fullprint << "myFunc=" << myFunc << std::endl;
    Point point(myFunc.getInputDimension(), 1.2);
    fullprint << "myFunc(point)=" << myFunc(point) << std::endl;
    fullprint << "myFunc input parameter(s)=";
    for (UnsignedInteger i = 0; i < myFunc.getInputDimension(); i++)
    {
      fullprint << (i == 0 ? "" : ",") << myFunc.getInputDescription()[i];
    }
    fullprint << std::endl;
    fullprint << "myFunc output parameter(s)=";
    for (UnsignedInteger i = 0; i < myFunc.getOutputDimension(); i++)
    {
      fullprint << (i == 0 ? "" : ",") << myFunc.getOutputDescription()[i];
    }
    fullprint << std::endl;
    for (UnsignedInteger i = 0; i < myFunc.getOutputDimension(); i++)
    {
      fullprint << "myFunc marginal " << i << "(point)=" << myFunc.getMarginal(i)(point) << std::endl;
    }
    Indices indices(2);
    indices[0] = 2;
    indices[1] = 0;
    fullprint << "myFunc marginal " << indices << "(point)=" << myFunc.getMarginal(indices)(point) << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}

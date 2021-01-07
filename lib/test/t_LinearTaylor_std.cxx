//                                               -*- C++ -*-
/**
 *  @brief The test file of class LinearTaylor for standard methods
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

  try
  {
    Scalar eps = 0.2;
    /** Instance creation */
    Description input(2);
    input[0] = "x1";
    input[1] = "x2";
    Description formula(3);
    formula[0] = "x1*sin(x2)";
    formula[1] = "cos(x1+x2)";
    formula[2] = "(x2+1)*exp(x1-2*x2)";
    SymbolicFunction myFunc(input, formula);
    Point center(myFunc.getInputDimension());
    for(UnsignedInteger i = 0; i < center.getDimension(); i++)
    {
      center[i] = 1.0 + i;
    }
    LinearTaylor myTaylor(center, myFunc);
    myTaylor.run();
    Function responseSurface(myTaylor.getMetaModel());
    fullprint << "myTaylor=" << myTaylor << std::endl;
    fullprint << "responseSurface=" << responseSurface << std::endl;
    fullprint << "myFunc(" << center << ")=" << myFunc(center) << std::endl;
    fullprint << "responseSurface(" << center << ")=" << responseSurface(center) << std::endl;
    Point in(center);
    in[0] += eps;
    in[1] -= eps / 2;
    fullprint << "myFunc(" << in << ")=" << myFunc(in) << std::endl;
    fullprint << "responseSurface(" << in << ")=" << responseSurface(in) << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}

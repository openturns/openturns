//                                               -*- C++ -*-
/**
 *  @brief The test file of class LinearLeastSquares for standard methods
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
    Description input(2);
    input[0] = "x1";
    input[1] = "x2";
    Description formula(3);
    formula[0] = "x1*sin(x2)";
    formula[1] = "cos(x1+x2)";
    formula[2] = "(x2+1)*exp(x1-2*x2)";
    SymbolicFunction myFunc(input, formula);
    Sample data(9, myFunc.getInputDimension());
    Point point(myFunc.getInputDimension());
    point[0] = 0.5;
    point[1] = 0.5;
    data[0] = point;
    point[0] = -0.5;
    point[1] = -0.5;
    data[1] = point;
    point[0] = -0.5;
    point[1] = 0.5;
    data[2] = point;
    point[0] = 0.5;
    point[1] = -0.5;
    data[3] = point;
    point[0] = 0.5;
    point[1] = 0.5;
    data[4] = point;
    point[0] = -0.25;
    point[1] = -0.25;
    data[5] = point;
    point[0] = -0.25;
    point[1] = 0.25;
    data[6] = point;
    point[0] = 0.25;
    point[1] = -0.25;
    data[7] = point;
    point[0] = 0.25;
    point[1] = 0.25;
    data[8] = point;
    {
      LinearLeastSquares myLeastSquares(data, myFunc);
      myLeastSquares.run();
      Function responseSurface(myLeastSquares.getMetaModel());
      fullprint << "myLeastSquares=" << myLeastSquares << std::endl;
      fullprint << "responseSurface=" << responseSurface << std::endl;
      Point in(myFunc.getInputDimension(), 0.1);
      fullprint << "myFunc(" << in << ")=" << myFunc(in) << std::endl;
      fullprint << "responseSurface(" << in << ")=" << responseSurface(in) << std::endl;
    }
    {
      Sample dataOut(myFunc(data));
      LinearLeastSquares myLeastSquares(data, dataOut);
      myLeastSquares.run();
      Function responseSurface(myLeastSquares.getMetaModel());
      fullprint << "myLeastSquares=" << myLeastSquares << std::endl;
      fullprint << "responseSurface=" << responseSurface << std::endl;
      Point in(myFunc.getInputDimension(), 0.1);
      fullprint << "myFunc(" << in << ")=" << myFunc(in) << std::endl;
      fullprint << "responseSurface(" << in << ")=" << responseSurface(in) << std::endl;
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}

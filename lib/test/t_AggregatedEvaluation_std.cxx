//                                               -*- C++ -*-
/**
 * @brief The test file of class AggregatedEvaluation for standard methods
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
    Description inputNames(3);
    inputNames[0] = "x0";
    inputNames[1] = "x1";
    inputNames[2] = "x2";
    Description formulas1(1);
    formulas1[0] = "x0^2+2*x1+3*x2^3";
    SymbolicFunction function1(inputNames, formulas1);
    Description formulas2(2);
    formulas2[0] = "cos(x0*sin(x2+x1))";
    formulas2[1] = "exp(x1 - x0 * sin(x2))";
    SymbolicFunction function2(inputNames, formulas2);
    Collection< Function > coll(2);
    coll[0] = function1;
    coll[1] = function2;
    AggregatedEvaluation evaluation(coll);
    fullprint << "evaluation=" << evaluation << std::endl;
    Point point(3);
    point[0] = 4.0;
    point[1] = -4.0;
    point[2] = 1.0;
    fullprint << "function 1 at" << point << "=" << function1(point) << std::endl;
    fullprint << "function 2 at" << point << "=" << function2(point) << std::endl;
    fullprint << "evaluation at" << point << "=" << evaluation(point) << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}

//                                               -*- C++ -*-
/**
 *  @brief The test file of class Function for linear combinations
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
    {
      // Product
      // First, build a function from R^3->R
      Description inVar(Description::BuildDefault(3, "x"));
      Description formula(1);
      formula[0] = "x0^2 + 2 * x1 * x2 + 3 * x2";
      SymbolicFunction f1(inVar, formula);
      // Second, build a function from R^3->R^2
      formula[0] = "x2 - x0 + x1";
      formula.add("x0 + x1 * x0 + x2");
      SymbolicFunction f2(inVar, formula);
      // Third, build the product function
      Function myFunction(f1 * f2);
      Point inPoint(3);
      inPoint[0] = 1.2;
      inPoint[1] = 2.3;
      inPoint[2] = 3.4;
      fullprint << "myFunction=" << myFunction << std::endl;
      fullprint << "myFunction=" << myFunction.__str__() << std::endl;
      const Point value(myFunction(inPoint));
      fullprint << "Value at " << inPoint << "=\n" << value << std::endl;
      fullprint << "Value at " << inPoint << "=\n" << value.__str__() << std::endl;
      const Matrix gradient(myFunction.gradient(inPoint));
      fullprint << "Gradient at " << inPoint << "=\n" << gradient << std::endl;
      fullprint << "Gradient at " << inPoint << "=\n" << gradient.__str__() << std::endl;
      const SymmetricTensor hessian(myFunction.hessian(inPoint));
      fullprint << "Hessian at " << inPoint << "=\n" << hessian << std::endl;
      fullprint << "Hessian at " << inPoint << "=\n" << hessian.__str__() << std::endl;
    } // Product
    {
      // Sum/difference
      // First, build two functions from R^3->R^2
      Description inVar(Description::BuildDefault(3, "x"));
      Description outVar(Description::BuildDefault(2, "y"));
      Description formula(2);
      formula[0] = "x0 + 2 * x1 * x2 + 3 * x2";
      formula[1] = "x2 - x0 + x1 * x0";
      SymbolicFunction f1(inVar, formula);
      formula[0] = "x0 + x1 + x2";
      formula[1] = "-2 * x0 + 3 * x2 * x1 - x1";
      SymbolicFunction f2(inVar, formula);
      Point inPoint(3);
      inPoint[0] = 1.2;
      inPoint[1] = 2.3;
      inPoint[2] = 3.4;
      // Second, build the function
      Function mySum(f1 + f2);
      {
        fullprint << "mySum=" << mySum << std::endl;
        fullprint << "mySum=" << mySum.__str__() << std::endl;
        const Point value(mySum(inPoint));
        fullprint << "Value at " << inPoint << "=\n" << value << std::endl;
        fullprint << "Value at " << inPoint << "=\n" << value.__str__() << std::endl;
        const Matrix gradient(mySum.gradient(inPoint));
        fullprint << "Gradient at " << inPoint << "=\n" << gradient << std::endl;
        fullprint << "Gradient at " << inPoint << "=\n" << gradient.__str__() << std::endl;
        const SymmetricTensor hessian(mySum.hessian(inPoint));
        fullprint << "Hessian at " << inPoint << "=\n" << hessian << std::endl;
        fullprint << "Hessian at " << inPoint << "=\n" << hessian.__str__() << std::endl;
      }
      Function myDiff(f1 - f2);
      {
        fullprint << "myDiff=" << myDiff << std::endl;
        fullprint << "myDiff=" << myDiff.__str__() << std::endl;
        const Point value(myDiff(inPoint));
        fullprint << "Value at " << inPoint << "=\n" << value << std::endl;
        fullprint << "Value at " << inPoint << "=\n" << value.__str__() << std::endl;
        const Matrix gradient(myDiff.gradient(inPoint));
        fullprint << "Gradient at " << inPoint << "=\n" << gradient << std::endl;
        fullprint << "Gradient at " << inPoint << "=\n" << gradient.__str__() << std::endl;
        const SymmetricTensor hessian(myDiff.hessian(inPoint));
        fullprint << "Hessian at " << inPoint << "=\n" << hessian << std::endl;
        fullprint << "Hessian at " << inPoint << "=\n" << hessian.__str__() << std::endl;
      }
    } // Sum/difference
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}

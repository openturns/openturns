//                                               -*- C++ -*-
/**
 *  @brief The test file of class IteratedQuadrature
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include <cmath>

using namespace OT;
using namespace OT::Test;

typedef Collection<Complex> ComplexCollection;

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {
    // First, compute the volume of the unit ball in R^n
    Scalar a = -1.0;
    Scalar b = 1.0;
    String formula("1.0");
    Collection< Function > lower(0);
    Collection< Function > upper(0);
    IteratedQuadrature algo(GaussKronrod(20, 1.0e-6, GaussKronrodRule(GaussKronrodRule::G3K7)));
    for (UnsignedInteger n = 0; n < 3; ++n)
    {
      Description inVars(Description::BuildDefault(n + 1, "x"));
      Description inVarsBounds(n);
      for (UnsignedInteger i = 0; i < n; ++i)
        inVarsBounds[i] = inVars[i];
      if (n > 0)
      {
        formula += String("-") + inVars[n - 1] + String("^2");
        lower.add(SymbolicFunction(inVarsBounds, Description(1, String("-sqrt(") + formula + String(")"))));
        upper.add(SymbolicFunction(inVarsBounds, Description(1, String("sqrt(") + formula + String(")"))));
      }
      SymbolicFunction integrand(inVars, Description(1, "1.0"));
      Scalar value = algo.integrate(integrand, a, b, lower, upper)[0];
      fullprint << "dim=" << n + 1 << ", volume=" << value << ", calls=" << integrand.getCallsNumber() << std::endl;
    }
    // Second, integrate a multi-valued function
    Interval bounds(Point(3, -1.0), Point(3, 1.0));
    Description vars(Description::BuildDefault(3, "x"));
    Description formulas(2);
    formulas[0] = "x0^2 + 2*x1^2 + 3*x2^2";
    formulas[1] = "x2^2 + 2*x1^2 + 3*x0^2";
    SymbolicFunction integrand(vars, formulas);
    Point value(algo.integrate(integrand, bounds));
    fullprint << "value=" << value << ", calls=" << integrand.getCallsNumber() << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}

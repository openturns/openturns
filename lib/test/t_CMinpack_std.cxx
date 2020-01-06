//                                               -*- C++ -*-
/**
 *  @brief The test file of class NLopt for standard methods
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
    UnsignedInteger n = 3;
    UnsignedInteger m = 10;
    Sample x(m, 1);
    for (UnsignedInteger i = 0; i < m; ++i) x(i, 0) = 0.5 + i;

    Description inVars(0);
    inVars.add("a");
    inVars.add("b");
    inVars.add("c");
    inVars.add("x");
    Description formulas(1, "a + b * exp(c * x)");
    SymbolicFunction model(inVars, formulas);
    Point p_ref(0);
    p_ref.add(2.8);
    p_ref.add(1.2);
    p_ref.add(0.5);
    Indices params(3);
    params.fill();
    ParametricFunction modelX(model, params, p_ref);
    Sample y = modelX(x);
    Collection<Function> components(0);
    UnsignedInteger precision = PlatformInfo::GetNumericalPrecision();
    PlatformInfo::SetNumericalPrecision(20);
    for (UnsignedInteger i = 0; i < x.getSize(); ++i)
    {
      ParametricFunction modelXi(model, Indices(1, 3), x[i]);
      components.add(modelXi - SymbolicFunction(modelXi.getInputDescription(), Description(1, String(OSS(true) << y(i, 0)))));
    }
    PlatformInfo::SetNumericalPrecision(precision);
    AggregatedFunction residualFunction(components);

    Point lower(3, 0.0);
    Point upper(0);
    upper.add(2.5);
    upper.add(8.0);
    upper.add(19.0);
    Interval bounds(lower, upper);

    for (UnsignedInteger bound = 0; bound < 2; ++bound)
    {
      LeastSquaresProblem problem(residualFunction);
      if (bound == 0) problem.setBounds(bounds);
      Point startingPoint(n, 1.0);
      CMinpack algo(problem);
      algo.setStartingPoint(startingPoint);
      algo.run();
      OptimizationResult result = algo.getResult();
      Point x_star = result.getOptimalPoint();
      fullprint << "Param opt=" << result.getOptimalPoint() << std::endl;
      if (bound == 0)
        assert_almost_equal(bounds.contains(x_star), true);
      else
        assert_almost_equal(x_star, p_ref);
    } // bound
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}

//                                               -*- C++ -*-
/**
 *  @brief The test file of class NLopt for standard methods
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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

  PlatformInfo::SetNumericalPrecision(3);

  try
  {
    // bounds
    const Description inVars = {"x1", "x2"};
    const Description formula(1, "10-5*(x1-3)^2-7*(x2-2)^2+0.1*(x1+x2)");

    const SymbolicFunction f(inVars, formula);

    const UnsignedInteger dim = f.getInputDimension();
    const Point startingPoint(dim, 0.0);

    const Interval bounds(Point(dim, -6.0), Point({1.0, 7.0}));

    const Description algoNames(NLopt::GetAlgorithmNames());
    for (UnsignedInteger i = 0; i < algoNames.getSize(); ++i)
    {
      NLopt algo(algoNames[i]);
      if (algoNames[i] == "GN_ISRES")
        algo.setMaximumConstraintError(1e-2);
      for (SignedInteger minimization = 0; minimization < 2; ++ minimization)
        for (SignedInteger inequality = 0; inequality < 2; ++ inequality)
          for (SignedInteger equality = 0; equality < 2; ++ equality)
            for(SignedInteger bound = 0; bound < 2; ++ bound)
            {
              if (minimization && !bound)
                continue;
              OptimizationProblem problem(f);
              problem.setMinimization(minimization == 1);
              if (inequality)
                // x1 <= 2
                problem.setInequalityConstraint(SymbolicFunction(inVars, Description(1, "2-x1")));
              if (equality)
                // x2 = 4
                problem.setEqualityConstraint(SymbolicFunction(inVars, Description(1, "x2-4")));
              if (bound)
                problem.setBounds(bounds);
              try
              {
                algo.setProblem(problem);
                algo.setStartingPoint(startingPoint);
                algo.setMaximumEvaluationNumber(1000);
                fullprint << "algo=" << algoNames[i] << " minimization=" << minimization << " bounds=" << bound << " inequality=" << inequality << " equality=" << equality << std::endl;
                algo.run();
              }
              catch (...)
              {
                fullprint << "-- Not supported: algo=" << algoNames[i] << " inequality=" << inequality << " equality=" << equality << std::endl;
                continue;
              }
              const OptimizationResult result(algo.getResult());
              const Point x(result.getOptimalPoint());
              fullprint << "x=" << x.__str__() << " y=" << result.getOptimalValue().__str__() << std::endl;
              if (!inequality && !equality)
              {
                if (!minimization)
                {
                  const Point x_ref = bound ? Point({1.0, 2.0}) : Point({3.0, 2.0});
                  assert_almost_equal(x, x_ref, 4e-1, 1e-2);
                }
                else
                {
                  const Point x_ref = {-6.0, -6.0};
                  // these go to (-6,7)
                  if ((algoNames[i] == "LN_NELDERMEAD") || (algoNames[i] == "LN_SBPLX"))
                    continue;
                  assert_almost_equal(x, x_ref, 4e-1, 1e-2);
                }
              }
              else if (equality)
                assert_almost_equal(x[1], 4.0, 4e-1, 1e-2);
              else if (inequality && x[0] > 2.01)
                throw TestFailed("!x1<=2");
            } // bound
    } // algo
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}

//                                               -*- C++ -*-
/**
 *  @brief The test file of class Bonmin
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
#include <float.h>

using namespace OT;
using namespace OT::Test;

void debug(const String & message,
           const String & where)
{
  std::cout << " DEBUG [" << where << "]: " << message << std::endl;
}


int main()
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {
    /* TEST ON SIMPLE FUNCTION */

    // Definition of objective function
    const Description inputVariables = {"x0", "x1", "x2", "x3"};
    const Description objective(1, "-x0 -x1 -x2");
    SymbolicFunction objectiveFunction(inputVariables, objective);

    // Definition of bounds
    const Point variablesLowerBounds(4, 0.0);
    const Point variablesUpperBounds = {1.0, DBL_MAX, DBL_MAX, 5.0};
    Collection<UnsignedInteger> variablesFiniteLowerBounds(4, 1);
    Collection<UnsignedInteger> variablesFiniteUpperBounds = {1, 0, 0, 1};
    Interval variablesBounds(variablesLowerBounds, variablesUpperBounds, variablesFiniteLowerBounds, variablesFiniteUpperBounds);

    // Definition of inequality constraints:
    // Bonmin constraints are defined as g_l <= g(x) <= g_u
    // OpenTURNS' are defined as g(x) >= 0
    const Description inequalityFormulas = {"-(x1 - 1/2)^2 - (x2 - 1/2)^2 + 1/4", "-x0 + x1", "-x0 - x2 - x3 + 2"};
    SymbolicFunction inequalityConstraints(inputVariables, inequalityFormulas);

    // Definition of variables types
    const Indices varTypes = {OT::OptimizationProblemImplementation::BINARY,
                              OT::OptimizationProblemImplementation::CONTINUOUS,
                              OT::OptimizationProblemImplementation::CONTINUOUS,
                              OT::OptimizationProblemImplementation::INTEGER};

    // Definition of OptimizationProblem
    OptimizationProblem problem(objectiveFunction);
    problem.setBounds(variablesBounds);
    problem.setVariablesType(varTypes);
    problem.setInequalityConstraint(inequalityConstraints);

    // Running Bonmin algorithm
    Bonmin bonminAlgorithm(problem);
    bonminAlgorithm.setStartingPoint(Point(4, 0));
    bonminAlgorithm.setMaximumCallsNumber(10000);
    bonminAlgorithm.setMaximumIterationNumber(1000);

    // Test __repr__ method
    std::cout << bonminAlgorithm.__repr__() << std::endl;


    // Defining reference solution
    const Point referenceSolution = {1.0, 1.0, 0.5, 0.0};

    // Defining solvers to test
    const Description solvers(Bonmin::GetAlgorithmNames());

    for (UnsignedInteger i = 0; i < solvers.getSize(); ++i)
    {
      // Set solver
      bonminAlgorithm.setAlgorithmName(solvers[i]);
      std::cout << " == TEST WITH " << bonminAlgorithm.getAlgorithmName() << " SOLVER:" << std::endl;

      // Run the solver
      bonminAlgorithm.run();

      // Retrieve solution
      OptimizationResult result(bonminAlgorithm.getResult());
      std::cout << " -- Optimal point = " << result.getOptimalPoint() << std::endl;
      std::cout << " -- Optimal value = " << result.getOptimalValue() << std::endl;
      std::cout << " -- Evaluation num = " << result.getInputSample().getSize() << std::endl;

      // Check result
      assert_almost_equal(result.getOptimalPoint(), referenceSolution, 1e-2);
      std::cout << " => TEST " << bonminAlgorithm.getAlgorithmName() << " PASSED" << std::endl << std::endl;
    }
  }

  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}

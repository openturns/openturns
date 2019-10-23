#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import print_function
import openturns as ot
import openturns.testing as ott
import sys


def progress(percent):
    sys.stderr.write('-- progress=' + str(percent) + '%\n')


def stop():
    sys.stderr.write('-- stop?\n')
    return False


# List available algorithms
for algo in ot.Bonmin.GetAlgorithmNames():
    print(algo)

# Definition of objective function
objectiveFunction = ot.SymbolicFunction(
    ['x0', 'x1', 'x2', 'x3'], ['-x0 -x1 -x2'])

# Definition of variables bounds
bounds = ot.Interval([0, 0, 0, 0], [1, 1e308, 1e308, 5], [
                     True, True, True, True], [True, False, False, True])

# Definition of constraints
# Constraints in OpenTURNS are defined as g(x) = 0 and h(x) >= 0
#    No equality constraint -> nothing to do
#    Inequality constraints:
h = ot.SymbolicFunction(['x0', 'x1', 'x2', 'x3'], [
                        '-(x1-1/2)^2 - (x2-1/2)^2 + 1/4', '-x0 + x1', '-x0 - x2 - x3 + 2'])

# Definition of variables types
variablesType = [ot.OptimizationProblemImplementation.BINARY, ot.OptimizationProblemImplementation.CONTINUOUS,
                 ot.OptimizationProblemImplementation.CONTINUOUS, ot.OptimizationProblemImplementation.INTEGER]

# Setting up Bonmin problem
problem = ot.OptimizationProblem(objectiveFunction)
problem.setBounds(bounds)
problem.setVariablesType(variablesType)
problem.setInequalityConstraint(h)

bonminAlgorithm = ot.Bonmin(problem, 'B-BB')
bonminAlgorithm.setStartingPoint([0, 0, 0, 0])
bonminAlgorithm.setMaximumEvaluationNumber(10000)
bonminAlgorithm.setProgressCallback(progress)
bonminAlgorithm.setStopCallback(stop)

ot.ResourceMap.SetAsScalar('Bonmin-bonmin.time_limit', 60)
ot.ResourceMap.SetAsString('Bonmin-mu_oracle', 'loqo')

algos = ot.Bonmin.GetAlgorithmNames()

for algo in algos:
    if algo != "B-iFP":  # Solver B-iFP fails to terminate on this case
        print("MINIMIZATION WITH " + algo)
        bonminAlgorithm.setAlgorithmName(algo)
        bonminAlgorithm.run()
        result = bonminAlgorithm.getResult()
        print(" -- Optimal point = " + result.getOptimalPoint().__str__())
        print(" -- Optimal value = " + result.getOptimalValue().__str__())
        print(" -- Evaluation number = " +
              result.getInputSample().getSize().__str__())
        ott.assert_almost_equal(result.getOptimalPoint(), [
                                1, 1, 0.5, 0], 1, 5e-4)

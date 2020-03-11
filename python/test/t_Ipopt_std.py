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


# Definition of objective function
f = ot.SymbolicFunction(['x1', 'x2'], ['-(x2 - 2.0) * (x2 - 2.0)'])

# Definition of variables bounds
bounds = ot.Interval([-1.0, -1.0e19], [1.0, 1.0e19], [True, True], [True, True])

# Definition of constraints
# Constraints in OpenTURNS are defined as g(x) = 0 and h(x) >= 0
#    No equality constraint -> nothing to do
#    Inequality constraints:
g = ot.SymbolicFunction(['x1', 'x2'], ['-(x1 * x1 + x2 - 1.0)'])

# Setting up problem
problem = ot.OptimizationProblem(f)
problem.setBounds(bounds)
problem.setEqualityConstraint(g)

algo = ot.Ipopt(problem)
algo.setStartingPoint([0.5, 1.5])
algo.setMaximumEvaluationNumber(10000)
algo.setProgressCallback(progress)
algo.setStopCallback(stop)

ot.ResourceMap.AddAsScalar('Ipopt-max_cpu_time', 15.0)

algo.run()
result = algo.getResult()
print(" -- Optimal point = " + result.getOptimalPoint().__str__())
print(" -- Optimal value = " + result.getOptimalValue().__str__())
print(" -- Evaluation number = " + result.getInputSample().getSize().__str__())
ott.assert_almost_equal(result.getOptimalPoint(), [1.0, 0.0], 1e-5, 1e-4)

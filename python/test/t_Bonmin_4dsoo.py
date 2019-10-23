# -*- coding: utf-8 -*-
"""
An optimization problem with mixed discrete input variables "4d soo".

min f(x1,x2,disc_b,logic)
where
x1 in [-5,5], x2 in [-3,3], disc_b in {True,False}, logic in {True,False}

4D function with a mixed decision space (two numeric, one discrete,
and one logical parameter)

In the original problem, we have disc_b in {"a","b"}.

Reference
Ported from R to Python.
Package ‘smoof’, August 14, 2017
Single and Multi-Objective Optimization Test Functions
Jakob Bossek
"""

import openturns as ot
import openturns.testing as ott


def fourdsoo(x):
    x1, x2, disc_b, logic = x
    # minimum : f([-5,-3,1,1]) = -18
    if round(disc_b) == 0.:
        result = x1 ** 2 + x2 ** 2 + 10 * round(logic)
    else:
        result = x1 + x2 - 10 * round(logic)
    return [result]


# Create the objective function
objectiveFun = ot.PythonFunction(4, 1, fourdsoo)

x = [-5, -3, 1, 1]

print("Evaluate f at x=", x)
print("f(x)=", objectiveFun(x))

# Define bounds
bounds = ot.Interval([-5., -3., 0., 0.], [5., 3., 1, 1])

# Define variable types
varTypes = [ot.OptimizationProblemImplementation.CONTINUOUS, ot.OptimizationProblemImplementation.CONTINUOUS,
            ot.OptimizationProblemImplementation.BINARY, ot.OptimizationProblemImplementation.INTEGER]

# Define problem
problem = ot.OptimizationProblem(objectiveFun)
problem.setVariablesType(varTypes)
problem.setBounds(bounds)
problem.setMinimization(True)

x0 = [0, 0, 0, 0]
algo = ot.Bonmin(problem, "B-BB")
algo.setStartingPoint(x0)
algo.setMaximumEvaluationNumber(10000)
algo.setMaximumIterationNumber(1000)
ot.ResourceMap.SetAsScalar('Bonmin-bonmin.time_limit', 60)
algo.run()

result = algo.getResult()
x_star = result.getOptimalPoint()
print("x*=", x_star)
y_star = result.getOptimalValue()
neval = result.getEvaluationNumber()
print("f(x*)=", y_star, "neval=", neval)


# ASSERTION
ott.assert_almost_equal(x_star, [-5, -3, 1, 1], 5e-4)

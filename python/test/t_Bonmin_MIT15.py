# -*- coding: utf-8 -*-
"""
An optimization problem with mixed discrete input variables.

Simple PLNE :

maximize     15*x + 12*y + 4*z + 2*t
subject to   -(8*x + 5*y + 3*z + 2*t - 10) >= 0

where x, y, z, t in {0,1}
Solution is [0, 1, 1, 1] with objectif value = 18

Without discrete constraint, we get :
x*= [0.625,1,0,0], f(x*)= [-21.375]

References
https://ocw.mit.edu/courses/sloan-school-of-management/15-053-optimization-methods-in-management-science-spring-2013/tutorials/MIT15_053S13_tut10.pdf
"""

import openturns as ot
import openturns.testing as ott

# Define the objective function
objectiveFun = ot.SymbolicFunction(
    ["x", "y", "z", "t"], ["-(15*x + 12*y + 4*z + 2*t)"])
constraintFun = ot.SymbolicFunction(
    ["x", "y", "z", "t"], ["-(8*x + 5*y + 3*z + 2*t -10)"])
x = [0, 1, 1, 1]
print("Evaluate f at x=", x)
print("f(x)=", objectiveFun(x))
print("g(x)=", constraintFun(x))

# Define problem
problem = ot.OptimizationProblem(objectiveFun)
problem.setInequalityConstraint(constraintFun)
bounds = ot.Interval([0., 0., 0., 0.], [1., 1., 1., 1.])
problem.setBounds(bounds)
problem.setMinimization(True)
problem.setVariablesType([ot.OptimizationProblemImplementation.BINARY, ot.OptimizationProblemImplementation.BINARY,
                          ot.OptimizationProblemImplementation.BINARY, ot.OptimizationProblemImplementation.BINARY])

# Define OptimizationAlgorithm
x0 = [0., 0., 0., 0.]
algo = ot.Bonmin(problem, "B-BB")
algo.setStartingPoint(x0)
algo.setMaximumEvaluationNumber(10000)
algo.setMaximumIterationNumber(1000)
ot.ResourceMap.SetAsScalar('Bonmin-bonmin.time_limit', 60)
algo.run()

# Retrieve result
result = algo.getResult()
x_star = result.getOptimalPoint()
print("x*=", x_star)
y_star = result.getOptimalValue()
neval = result.getEvaluationNumber()
print("f(x*)=", y_star, "neval=", neval)

print("g(x*)=", constraintFun(x_star))


# ASSERTION
ott.assert_almost_equal(x_star, [0, 1, 1, 1], 1, 5e-4)

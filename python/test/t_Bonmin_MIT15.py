#!/usr/bin/env python
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
objectiveFun = ot.SymbolicFunction(["x", "y", "z", "t"], ["-(15*x + 12*y + 4*z + 2*t)"])
constraintFun = ot.SymbolicFunction(
    ["x", "y", "z", "t"], ["-(8*x + 5*y + 3*z + 2*t -10)"]
)
x = [0, 1, 1, 1]
print(f"f(x)={objectiveFun(x)}")
print(f"g(x)={constraintFun(x)}")

# Define problem
problem = ot.OptimizationProblem(objectiveFun)
problem.setInequalityConstraint(constraintFun)
bounds = ot.Interval([0.0] * 4, [1.0] * 4)
problem.setBounds(bounds)
problem.setMinimization(True)
problem.setVariablesType(
    [
        ot.OptimizationProblemImplementation.BINARY,
        ot.OptimizationProblemImplementation.BINARY,
        ot.OptimizationProblemImplementation.BINARY,
        ot.OptimizationProblemImplementation.BINARY,
    ]
)

# Define OptimizationAlgorithm
x0 = [0.0] * 4
algo = ot.Bonmin(problem)
algo.setStartingPoint(x0)
algo.setMaximumCallsNumber(10000)
algo.setMaximumIterationNumber(1000)


for name in ot.Bonmin.GetAlgorithmNames():
    print(f"-- {name} algorithm...")
    algo.setAlgorithmName(name)
    algo.run()

    # Retrieve result
    result = algo.getResult()
    x_star = result.getOptimalPoint()
    print("x*=", x_star)
    y_star = result.getOptimalValue()
    neval = result.getCallsNumber()
    print(f"f(x*)={y_star} neval={neval}")
    print(f"g(x*)={constraintFun(x_star)}")
    ott.assert_almost_equal(x_star, [0, 1, 1, 1], 1, 5e-4)

# -*- coding: utf-8 -*-
"""
An optimization problem with mixed discrete input variables.

min f(x1,x2,x3)
where
x1 is discrete in {0,1,2,3,4}
x2 and x3 are real in [0,1]

Mixed parameter space with one discrete parameter \eqn{x_1 \in \{0, 1, 2, 3, 4\}}
and two numerical parameters \eqn{x_1, x_2 \in [0, 1]}.
The function is defined as follows:
f(\mathbf{x}) = \\
\sin(2\pi x_3 - \pi) + 7 \sin^2(2 \pi x_2 - \pi) \, if \, x_1 = 0 \\
\sin(2\pi x_3 - \pi) + 7 \sin^2(2 \pi x_2 - \pi) + 12 \sin(2 \pi x_3 - \pi) \, if \, x_1 = 1 \\
\sin(2\pi x_3 - \pi) + 7 \sin^2(2 \pi x_2 - \pi) + 0.5 \sin(2 \pi x_3 - \pi) \, if \, x_1 = 2 \\
\sin(2\pi x_3 - \pi) + 7 \sin^2(2 \pi x_2 - \pi) + 8.0 \sin(2 \pi x_3 - \pi) \, if \, x_1 = 3 \\
\sin(2\pi x_3 - \pi) + 7 \sin^2(2 \pi x_2 - \pi) + 3.5 \sin(2 \pi x_3 - \pi) \, if \, x_1 = 4.

Reference
Ported from R to Python, based on
https://rdrr.io/cran/smoof/src/R/sof.swiler2014.R

"""

import math
import openturns as ot
import openturns.testing as ott


def swiler2014(x):
    x1, x2, x3 = x
    # minimum  = f([1, 0 , 0.25])=-13

    a = math.sin(2*math.pi*x3 - math.pi)
    b = 7 * math.sin(2*math.pi*x2 - math.pi)**2
    facs = [0, 12.0, 0.5, 8.0, 3.5]  # A set of numeric values
    fac = facs[int(x1)]  # Convert to int, then use the value in the list
    result = a + b + fac * a
    # print("x=",[x1,x2,x3],"f(x)=",result)
    return [result]


# Create the objective function
objectiveFun = ot.PythonFunction(3, 1, swiler2014)
x = [1, 0, 0.25]
print("Evaluate f at x=", x)
print("f(x)=", objectiveFun(x))

# Define OptimizationProblem
problem = ot.OptimizationProblem(objectiveFun)
bounds = ot.Interval([0., 0., 0], [1., 1., 4])
varTypes = [ot.OptimizationProblemImplementation.INTEGER,
            ot.OptimizationProblemImplementation.CONTINUOUS, ot.OptimizationProblemImplementation.CONTINUOUS]
problem.setBounds(bounds)
problem.setVariablesType(varTypes)
problem.setMinimization(True)

# Define OptimizationAlgorithm
x0 = [0]*3
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


# ASSERTIONS
ott.assert_almost_equal(x_star, [1.0, 0.0, 0.25], 1, 5e-4)

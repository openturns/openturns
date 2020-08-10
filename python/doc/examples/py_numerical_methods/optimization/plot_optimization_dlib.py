# ---
# jupyter:
#   jupytext:
#     text_representation:
#       extension: .py
#       format_name: percent
#       format_version: '1.3'
#       jupytext_version: 1.5.1
#   kernelspec:
#     display_name: Python 3
#     language: python
#     name: python3
# ---

# %%
"""
Optimization using dlib
=======================
"""
# %% 

# %%
# In this example we are going to explore optimization using OpenTURNS' [dlib](http://dlib.net/) interface.

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt

# %%
# List available algorithms
for algo in ot.Dlib.GetAlgorithmNames():
    print(algo)

# %%
# More details on dlib algorithms are available [here](http://dlib.net/optimization.html).

# %%
# Solving an unconstrained problem with conjugate gradient algorithm
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#
# The following example will demonstrate the use of dlib conjugate gradient algorithm to find the minimum of [Rosenbrock function](https://en.wikipedia.org/wiki/Rosenbrock_function). The optimal point can be computed analytically, and its value is [1.0, 1.0].

# %%
# Define the problem based on Rosebrock function
rosenbrock = ot.SymbolicFunction(['x1', 'x2'], ['(1-x1)^2+(x2-x1^2)^2'])
problem = ot.OptimizationProblem(rosenbrock)

# %%
# The optimization algorithm is instanciated from the problem to solve and the name of the algorithm
algo = ot.Dlib(problem,'CG')
print("Dlib algorithm, type ", algo.getAlgorithmName())
print("Maximum iteration number: ", algo.getMaximumIterationNumber())
print("Maximum evaluation number: ", algo.getMaximumEvaluationNumber())
print("Maximum absolute error: ", algo.getMaximumAbsoluteError())
print("Maximum relative error: ", algo.getMaximumRelativeError())
print("Maximum residual error: ", algo.getMaximumResidualError())
print("Maximum constraint error: ", algo.getMaximumConstraintError())

# %%
# When using conjugate gradient, BFGS/LBFGS, Newton, least squares or trust region methods, optimization proceeds until one of the following criteria is met:
#
# - the errors (absolute, relative, residual, constraint) are all below the limits set by the user ;
# - the process reaches the maximum number of iterations or function evaluations.

# %%
# Adjust number of iterations/evaluations
algo.setMaximumIterationNumber(1000)
algo.setMaximumEvaluationNumber(10000)
algo.setMaximumAbsoluteError(1e-3)
algo.setMaximumRelativeError(1e-3)
algo.setMaximumResidualError(1e-3)

# %%
# Solve the problem
startingPoint = [1.5, 0.5]
algo.setStartingPoint(startingPoint)

algo.run()

# %%
# Retrieve results
result = algo.getResult()
print('x^ = ', result.getOptimalPoint())
print("f(x^) = ", result.getOptimalValue())
print("Iteration number: ", result.getIterationNumber())
print("Evaluation number: ", result.getEvaluationNumber())
print("Absolute error: ", result.getAbsoluteError())
print("Relative error: ", result.getRelativeError())
print("Residual error: ", result.getResidualError())
print("Constraint error: ", result.getConstraintError())

# %%
# Solving problem with bounds, using LBFGS strategy
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#
# In the following example, the input variables will be bounded so the function global optimal point is not included in the search interval.
#
# The problem will be solved using LBFGS strategy, which allows the user to limit the amount of memory used by the optimization process.

# %%
# Define the bounds and the problem
bounds = ot.Interval([0.0, 0.0], [0.8, 2.0])
boundedProblem = ot.OptimizationProblem(rosenbrock,ot.Function(),ot.Function(),bounds)

# %%
# Define the Dlib algorithm
boundedAlgo = ot.Dlib(boundedProblem,"LBFGS")
boundedAlgo.setMaxSize(15) # Default value for LBFGS' maxSize parameter is 10

startingPoint = [0.5, 1.5]
boundedAlgo.setStartingPoint(startingPoint)

boundedAlgo.run()

# %%
# Retrieve results
result = boundedAlgo.getResult()
print('x^ = ', result.getOptimalPoint())
print("f(x^) = ", result.getOptimalValue())
print("Iteration number: ", result.getIterationNumber())
print("Evaluation number: ", result.getEvaluationNumber())
print("Absolute error: ", result.getAbsoluteError())
print("Relative error: ", result.getRelativeError())
print("Residual error: ", result.getResidualError())
print("Constraint error: ", result.getConstraintError())

# %%
# **Remark:**
# The bounds defined for input variables are always strictly respected when using dlib algorithms. Consequently, the constraint error is always 0.

# %%
# Draw optimal value history
result.drawOptimalValueHistory()

# %%
# Solving least squares problem
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#
# In least squares problem, the user provides the residual function to minimize. Here the underlying OptimizationProblem is defined as a LeastSquaresProblem.
#
# dlib least squares algorithms use the same stop criteria as CG, BFGS/LBFGS and Newton algorithms. However, optimization will stop earlier if no significant improvement can be achieved during the process.

# %%
# Define residual function
import numpy as np
n = 3
m = 20
x = [[0.5 + 0.1*i] for i in range(m)]

model = ot.SymbolicFunction(['a', 'b', 'c', 'x'], ['a + b * exp(-c *x^2)'])
p_ref = [2.8, 1.2, 0.5] # Reference a, b, c
modelx = ot.ParametricFunction(model, [0, 1, 2], p_ref)

# Generate reference sample (with normal noise)
y = np.multiply(modelx(x), np.random.normal(1.0,0.05,m))

def residualFunction(params):
    modelx = ot.ParametricFunction(model, [0, 1, 2], params)
    return [modelx(x[i])[0] - y[i, 0] for i in range(m)]


# %%
# Definition of residual as ot.PythonFunction and optimization problem
residual = ot.PythonFunction(n,m,residualFunction)
lsqProblem = ot.LeastSquaresProblem(residual)

# %%
# Definition of Dlib solver, setting starting point
lsqAlgo = ot.Dlib(lsqProblem, "LSQ")
lsqAlgo.setStartingPoint([0.0,0.0,0.0])

lsqAlgo.run()

# %%
# Retrieve results
result = lsqAlgo.getResult()
print('x^ = ', result.getOptimalPoint())
print("f(x^) = ", result.getOptimalValue())
print("Iteration number: ", result.getIterationNumber())
print("Evaluation number: ", result.getEvaluationNumber())
print("Absolute error: ", result.getAbsoluteError())
print("Relative error: ", result.getRelativeError())
print("Residual error: ", result.getResidualError())
print("Constraint error: ", result.getConstraintError())

# %%
# Draw errors history
result.drawErrorHistory()

# %%
# Draw optimal value history
result.drawOptimalValueHistory()

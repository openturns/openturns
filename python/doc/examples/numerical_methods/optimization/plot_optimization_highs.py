"""
Optimization using HiGHS solver
===============================
"""

# %%

# %%
# In this example we are going to explore mixed-integer non linear problems
# optimization using the `HiGHS <https://highs.dev/>`_
# interface.
# %%
import openturns as ot
import openturns.experimental as otexp

# %%
# The following example will demonstrate the use of HiGHS solver to
# solve the following problem:
#
# .. math::
#    \max x +   y + 2 z
#
# subject to:
#
# .. math::
#    \begin{array}{l}
#    x + 2 y + 3 z \leq 4\\
#    x + y \ge 1\\
#    x, x, z \in \{0,1\}^3\\
#    \end{array}
#


# %%
# Definition of objective function as a cost vector
cost = [1, 1, 2]

# %%
# Definition of constraints in the form L<=Ax<=U
LU = ot.Interval([-1e30, 1], [4, 1e30])
A = ot.Matrix([[1, 2, 3], [1, 1, 0]])

# %%
# Definition of variables types
BINARY = ot.OptimizationProblemImplementation.BINARY
variablesType = [BINARY] * 3
bounds = ot.Interval(3)

# %%
# Define the linear problem
problem = otexp.LinearProblem(cost, bounds, A, LU)
problem.setVariablesType(variablesType)
problem.setMinimization(False)

# %%
# Run the algorithm
algo = otexp.HiGHS(problem)
algo.setStartingPoint([0.0] * 3)

# %%
# Running the solver
algo.run()

# Retrieving the results
result = algo.getResult()
print(" -- Optimal point = " + str(result.getOptimalPoint()))
print(" -- Optimal value = " + str(result.getOptimalValue()))
print(" -- Evaluation number = " + str(result.getInputSample().getSize()))

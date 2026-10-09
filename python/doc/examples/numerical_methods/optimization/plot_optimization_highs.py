"""
Optimization using HiGHS solvers
================================
"""

# %%
# In this example, we solve linear optimization problems using the
# `HiGHS <https://highs.dev/>`_ interface.
# First, we solve a mixed-integer problem with the default solver,
# then a continuous problem with several LP solvers.
import openturns as ot
import openturns.experimental as otexp

# %%
# Solve a mixed-integer problem
# -----------------------------
#
# We want to solve the following problem:
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
#    x, y, z \in \{0,1\}^3\\
#    \end{array}
#

# %%
# Define the mixed-integer linear problem.
# The cost vector, the matrix A and the bounds of the
# L <= A*x <= U constraints are defined below.
# Variables are binary and the problem is maximized.
cost = [1.0, 1.0, 2.0]
LU = ot.Interval([-1e30, 1.0], [4.0, 1e30])
A = ot.Matrix([[1.0, 2.0, 3.0], [1.0, 1.0, 0.0]])
BINARY = ot.OptimizationProblemImplementation.BINARY
bounds = ot.Interval(3)
problem = otexp.LinearProblem(cost, bounds, A, LU)
problem.setVariablesType([BINARY] * 3)
problem.setMinimization(False)

# %%
# Run the algorithm with default settings
algo = otexp.HiGHS(problem)
algo.setStartingPoint([0.0] * 3)
algo.run()

# %%
# Retrieve the results
result = algo.getResult()
print(" -- Optimal point = " + str(result.getOptimalPoint()))
print(" -- Optimal value = " + str(result.getOptimalValue()))
print(" -- Evaluation number = " + str(result.getInputSample().getSize()))

# %%
# Solve a continuous problem with several LP solvers
# --------------------------------------------------
#
# On continuous linear problems the LP solver can be selected
# among the recognized names returned by :func:`~openturns.experimental.HiGHS.GetAlgorithmNames`.
# (whether a name is accepted at runtime depends on the HiGHS build).
# Let us solve the following problem with several solvers:
#
# .. math::
#    \min x + y
#
# subject to:
#
# .. math::
#    \begin{array}{l}
#    x + 2 y \geq 6\\
#    x \geq 1\\
#    y \geq 0\\
#    \end{array}
#
print(" -- Recognized LP solver names = " + str(otexp.HiGHS.GetAlgorithmNames()))

# %%
# Definition of the continuous linear problem
bounds2 = ot.Interval([1.0, 0.0], [1e30, 1e30])
A2 = ot.Matrix([[1.0, 2.0]])
cb2 = ot.Interval([6.0], [1e30])
problem2 = otexp.LinearProblem([1.0, 1.0], bounds2, A2, cb2)

# %%
# Solve with each LP solver
for solver in ["choose", "simplex", "ipm"]:
    algo2 = otexp.HiGHS(problem2, solver)
    algo2.setMaximumConstraintError(0.0)
    algo2.run()
    result2 = algo2.getResult()
    print(
        " -- ["
        + solver
        + "] point = "
        + str(result2.getOptimalPoint())
        + " value = "
        + str(result2.getOptimalValue())
        + " status = "
        + result2.getStatusMessage()
    )

# %%
# The last solve also exposes the dual solution:
# constraint duals and per-variable reduced costs
print(" -- Duals = " + str(algo2.getDualPoint()))
print(" -- Reduced costs = " + str(algo2.getReducedCosts()))

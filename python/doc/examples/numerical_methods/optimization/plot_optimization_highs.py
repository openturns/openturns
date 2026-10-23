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
# The cost vector :math:`\vect{c}`, the matrix :math:`\mat{A}` and the bounds of the
# :math:`\vect{l} \leq \mat{A}\vect{x} \leq \vect{u}` constraints are defined below.
# Variables are binary and the problem is maximized.
# Infinite bounds are marked with finite flags set to ``False``
# (values such as ``1e30`` are then placeholders for :math:`\pm\infty`).
cost = [1.0, 1.0, 2.0]
LU = ot.Interval([-1e30, 1.0], [4.0, 1e30], [False, True], [True, False])
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
# among the recognized names returned by :func:`~openturns.experimental.HiGHS.GetAlgorithmNames`
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
# Unbounded variables and constraints use finite flags set to ``False``,
# with ``1e30`` as placeholder value for :math:`+\infty`.
bounds2 = ot.Interval([1.0, 0.0], [1e30, 1e30], [True, True], [False, False])
A2 = ot.Matrix([[1.0, 2.0]])
cb2 = ot.Interval([6.0], [1e30], [True], [False])
problem2 = otexp.LinearProblem([1.0, 1.0], bounds2, A2, cb2)

# %%
# Solve with each LP solver
# HiGHS solves the LP directly without OpenTURNS evaluations,
# so error histories stay empty (no convergence curve to draw);
# solvers are compared on final point, value, status and per-solver iterations.
for solver in otexp.HiGHS.GetAlgorithmNames():
    try:
        algo2 = otexp.HiGHS(problem2, solver)
        algo2.setMaximumConstraintError(0.0)
        algo2.run()
    except Exception as ex:
        print(" -- [" + solver + "] skipped: " + str(ex).splitlines()[0])
        continue
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
        + " iterations = "
        + str(result2.getIterationNumber())
        + " (simplex="
        + str(algo2.getSimplexIterationNumber())
        + ", ipm="
        + str(algo2.getIPMIterationNumber())
        + ", crossover="
        + str(algo2.getCrossoverIterationNumber())
        + ", pdlp="
        + str(algo2.getPDLPIterationNumber())
        + ", qp="
        + str(algo2.getQPIterationNumber())
        + ")"
    )

# %%
# A basic-solution solver also exposes the dual solution:
# constraint duals and per-variable reduced costs
# (first-order solvers without crossover have no dual to expose).
algoDual = otexp.HiGHS(problem2, "simplex")
algoDual.run()
print(" -- Duals = " + str(algoDual.getDualPoint()))
print(" -- Reduced costs = " + str(algoDual.getReducedCosts()))

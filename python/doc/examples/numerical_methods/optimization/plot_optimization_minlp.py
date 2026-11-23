"""
Mixed-integer nonlinear optimization
====================================

This example solves the reference problems of the :class:`~openturns.Bonmin`
interface with every mixed-integer solver available in OpenTURNS:

- `Bonmin <https://www.coin-or.org/Bonmin/index.html>`_,
- :class:`~openturns.experimental.SCIP`, available in the experimental module.

Both interfaces are optional: a solver section only runs when OpenTURNS has
been built with the corresponding library. The availability is tested with
:meth:`~openturns.PlatformInfo.HasFeature`.
"""
import math

import openturns as ot
import openturns.experimental as otexp

CONTINUOUS = ot.OptimizationProblemImplementation.CONTINUOUS
INTEGER = ot.OptimizationProblemImplementation.INTEGER
BINARY = ot.OptimizationProblemImplementation.BINARY

solvers = []
if ot.PlatformInfo.HasFeature("bonmin"):
    solvers.append("Bonmin")
if ot.PlatformInfo.HasFeature("scip"):
    solvers.append("SCIP")
print(" -- Available MINLP solvers = " + str(solvers))


def printResult(name, result):
    """Print the outcome of one solve."""
    print(f" -- [{name}] optimal point = {result.getOptimalPoint()}")
    print(f" -- [{name}] optimal value = {result.getOptimalValue()}")
    print(f" -- [{name}] status = {result.getStatusMessage()}")
    print(f" -- [{name}] evaluation number = {result.getCallsNumber()}")


def solveWithBonmin(problem, startingPoint):
    """Solve a problem with the default Bonmin branch-and-bound algorithm."""
    algorithm = ot.Bonmin(problem, "B-BB")
    algorithm.setStartingPoint(startingPoint)
    algorithm.setMaximumCallsNumber(1000000)
    algorithm.setMaximumIterationNumber(10000)
    algorithm.run()
    return algorithm.getResult()


def solveWithSCIP(problem, startingPoint, checkStatus=True):
    """Solve a problem with SCIP."""
    algorithm = otexp.SCIP(problem)
    # On a black-box problem SCIP stops on its node limit: the caller disables
    # the status check and inspects the result instead.
    algorithm.setCheckStatus(checkStatus)
    algorithm.setStartingPoint(startingPoint)
    algorithm.setMaximumCallsNumber(1000000)
    algorithm.setMaximumIterationNumber(10000)
    algorithm.run()
    return algorithm.getResult()


# %%
# A binary knapsack
# ^^^^^^^^^^^^^^^^^
#
# The first problem is a binary knapsack:
#
# .. math::
#    \max_{x \in \{0,1\}^4} 15 x_0 + 12 x_1 + 4 x_2 + 2 x_3
#
# such that:
#
# .. math::
#    8 x_0 + 5 x_1 + 3 x_2 + 2 x_3 \leq 10
#
# The optimum is :math:`x = [0,1,1,1]`, with an objective value of 18.
#
# Since the objective and the constraint are linear, the problem is described
# with a :class:`~openturns.experimental.LinearProgrammingProblem`, whose constraints live
# in the matrix form :math:`\vect{l} \leq \mat{A}\vect{x} \leq \vect{u}`.
# OpenTURNS minimizes, hence the negative costs.
cost = [-15.0, -12.0, -4.0, -2.0]
variableBounds = ot.Interval([0.0] * 4, [1.0] * 4)
A = ot.Matrix([[8.0, 5.0, 3.0, 2.0]])
constraintBounds = ot.Interval([0.0], [10.0])
linearProblem = otexp.LinearProgrammingProblem(cost, variableBounds, A, constraintBounds)
linearProblem.setVariablesType([BINARY] * 4)
problem = ot.OptimizationProblem(linearProblem)
startingPoint = [0.0] * 4

# %%
# Solve with Bonmin
# """""""""""""""""
#
# Bonmin reads the matrix form and proves optimality.
if "Bonmin" in solvers:
    printResult("Bonmin", solveWithBonmin(problem, startingPoint))

# %%
# Solve with SCIP
# """""""""""""""
#
# SCIP also keeps the matrix form: it builds the LP relaxation, branches on the
# binary variables and proves optimality.
if "SCIP" in solvers:
    printResult("SCIP", solveWithSCIP(problem, startingPoint))

# %%
# A mixed-integer problem with a quadratic constraint
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#
# We now solve the reference MINLP of the Bonmin interface:
#
# .. math::
#    \min - x_0 - x_1 - x_2
#
# such that:
#
# .. math::
#    \begin{array}{l}
#    (x_1 - \frac{1}{2})^2 + (x_2 - \frac{1}{2})^2 \leq \frac{1}{4} \\
#    x_0 - x_1 \leq 0 \\
#    x_0 + x_2 + x_3 \leq 2\\
#    x_0 \in \{0,1\}\\
#    (x_1, x_2) \in \mathbb{R}^2\\
#    x_3 \in \mathbb{N}
#    \end{array}
#
# The theoretical minimum is reached for :math:`x = [1,1,0.5,0]`.
# At this point, the objective function value is :math:`-2.5`.
#
# N.B.: equality and inequality constraints are required to be stated as
# :math:`g(x) = 0` and :math:`h(x) \geq 0`, respectively. Thus the inequalities
# above have to be restated to match this requirement:
#
# .. math::
#    \begin{array}{l}
#    -(x_1 - \frac{1}{2})^2 - (x_2 - \frac{1}{2})^2 + \frac{1}{4} \geq 0\\
#    -x_0 + x_1 \geq 0 \\
#    -x_0 - x_2 - x_3 + 2 \geq 0
#    \end{array}

# %%
# Definition of objective function
objectiveFunction = ot.SymbolicFunction(["x0", "x1", "x2", "x3"], ["-x0 - x1 - x2"])

# Definition of variables bounds
# The disk constraint already implies x1 <= 1 and x2 <= 1: keep these finite
# box bounds, they are the only domain information a black-box solver can use.
bounds = ot.Interval([0.0] * 4, [1.0, 1.0, 1.0, 5.0])

# Definition of constraints
# Constraints are defined as g(x) = 0 and h(x) >= 0
#    No equality constraint -> nothing to do
#    Inequality constraints:
h = ot.SymbolicFunction(
    ["x0", "x1", "x2", "x3"],
    ["-(x1-0.5)^2 - (x2-0.5)^2 + 0.25", "x1 - x0", "-x0 - x2 - x3 + 2"],
)

# Definition of variables types
variablesType = [BINARY, CONTINUOUS, CONTINUOUS, INTEGER]

# Setting up the problem
problem = ot.OptimizationProblem(objectiveFunction)
problem.setBounds(bounds)
problem.setVariablesType(variablesType)
problem.setInequalityConstraint(h)
startingPoint = [0.0] * 4

# %%
# Solve with Bonmin
# """""""""""""""""
#
# Bonmin solves the nonlinear relaxations and reports that the returned point
# is optimal.
if "Bonmin" in solvers:
    printResult("Bonmin", solveWithBonmin(problem, startingPoint))

# %%
# Solve with SCIP
# """""""""""""""
#
# SCIP receives the objective and the constraint as black-box functions: it can
# evaluate them and their derivatives, but it cannot build a relaxation. The
# auxiliary variable that carries the objective value is therefore unbounded
# below, and the search reduces to spatial branching on the variables. SCIP
# stops on its node limit and reports its best incumbent, which is not
# guaranteed to be optimal. The status check is disabled accordingly: the
# status message still reports the node limit, and OpenTURNS logs a warning
# that is expected here.
if "SCIP" in solvers:
    printResult("SCIP", solveWithSCIP(problem, startingPoint, checkStatus=False))

# %%
# A problem with discrete choices
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#
# The third problem mixes continuous and discrete decision variables:
#
# .. math::
#    f(x_1, x_2, d, l) =
#    \begin{cases}
#    x_1^2 + x_2^2 + 10 l & \text{if } d = 0,\\
#    x_1 + x_2 - 10 l & \text{otherwise},
#    \end{cases}
#
# with :math:`x_1 \in [-5,5]`, :math:`x_2 \in [-3,3]`, :math:`d \in \{0,1\}`
# and :math:`l \in \{0,1\}`. The minimum is reached for
# :math:`x = [-5,-3,1,1]`, with :math:`f(x) = -18`.
#
# The objective is a :class:`~openturns.PythonFunction`: neither solver can
# read it as an algebraic expression, so the derivatives are computed by
# finite differences.


def fourdsoo(x):
    x1, x2, disc_b, logic = x
    if round(disc_b) == 0.0:
        return [x1**2 + x2**2 + 10 * round(logic)]
    return [x1 + x2 - 10 * round(logic)]


objectiveFunction = ot.PythonFunction(4, 1, fourdsoo)
problem = ot.OptimizationProblem(objectiveFunction)
problem.setBounds(ot.Interval([-5.0, -3.0, 0.0, 0.0], [5.0, 3.0, 1.0, 1.0]))
problem.setVariablesType([CONTINUOUS, CONTINUOUS, BINARY, INTEGER])
startingPoint = [0.0] * 4

# %%
# Solve with Bonmin
# """""""""""""""""
#
# Bonmin evaluates the function through the black-box interface and returns
# the optimum with a success status.
if "Bonmin" in solvers:
    printResult("Bonmin", solveWithBonmin(problem, startingPoint))

# %%
# Solve with SCIP
# """""""""""""""
#
# SCIP explores the box with spatial branching. It reaches the optimal value
# here, but it cannot prove it because the objective is a black box.
if "SCIP" in solvers:
    printResult("SCIP", solveWithSCIP(problem, startingPoint, checkStatus=False))

# %%
# A multimodal problem
# ^^^^^^^^^^^^^^^^^^^^
#
# The last problem is a variant of the Swiler 2014 test function:
#
# .. math::
#    f(x_1, x_2, x_3) = \sin(2\pi x_3 - \pi)
#    + 7 \sin^2(2\pi x_2 - \pi)
#    + c(x_1) \sin(2\pi x_3 - \pi)
#
# where :math:`x_1 \in \{0,1,2,3,4\}` selects the coefficient
# :math:`c(x_1) \in \{0, 12, 0.5, 8, 3.5\}`, :math:`x_2 \in [0,1]` and
# :math:`x_3 \in [0,4]`. The box bound used below restricts :math:`x_1` to
# :math:`\{0,1\}`, so only the coefficients :math:`0` and :math:`12` occur.
# The minimum value is :math:`-13`, reached for instance at
# :math:`x = [1,0,0.25]`.
#
# The function is periodic and multimodal, which makes it a difficult case for
# a black-box solver.


def swiler2014(x):
    x1, x2, x3 = x
    a = math.sin(2 * math.pi * x3 - math.pi)
    b = 7 * math.sin(2 * math.pi * x2 - math.pi) ** 2
    fac = [0.0, 12.0, 0.5, 8.0, 3.5][int(round(x1))]
    return [a + b + fac * a]


objectiveFunction = ot.PythonFunction(3, 1, swiler2014)
problem = ot.OptimizationProblem(objectiveFunction)
problem.setBounds(ot.Interval([0.0] * 3, [1.0, 1.0, 4.0]))
problem.setVariablesType([INTEGER, CONTINUOUS, CONTINUOUS])
startingPoint = [0.0] * 3

# %%
# Solve with Bonmin
# """""""""""""""""
if "Bonmin" in solvers:
    printResult("Bonmin", solveWithBonmin(problem, startingPoint))

# %%
# Solve with SCIP
# """""""""""""""
#
# SCIP reaches the optimal value as well. Because the function is periodic,
# the returned point may differ from Bonmin's: several points are equivalent.
# As above, optimality is not proven since the objective is a black box.
if "SCIP" in solvers:
    printResult("SCIP", solveWithSCIP(problem, startingPoint, checkStatus=False))

# %%
# In summary, both solvers handle the matrix form exactly and prove
# optimality on the knapsack. On the black-box problems, Bonmin reports
# optimality, while SCIP can only explore the box and reports an incumbent
# when its node budget is exhausted.
if not solvers:
    print(" -- No MINLP solver available in this build of OpenTURNS")

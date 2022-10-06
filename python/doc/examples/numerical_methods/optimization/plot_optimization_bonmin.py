"""
Optimization using bonmin
=========================
"""
# %%

# %%
# In this example we are going to explore mixed-integer non linear problems
# optimization using the `bonmin <https://www.coin-or.org/Bonmin/index.html>`_
# interface.
# %%
import openturns as ot
ot.Log.Show(ot.Log.NONE)

# %%
# List available algorithms
for algo in ot.Bonmin.GetAlgorithmNames():
    print(algo)

# %%
# Details and references on bonmin algorithms are available
# `here <https://projects.coin-or.org/Bonmin>`_ .

# %%
# Setting up and solving a simple problem
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#
# The following example will demonstrate the use of bonmin "BB" algorithm to
# solve the following problem:
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
#    x_0 \in \{0,1\}^n\\
#    (x_1, x_2) \in \mathbb{R}^2\\
#    x_3 \in \mathbb{N}
#    \end{array}
#
# The theoretical minimum is reached for :math:`x = [1,1,0.5,0]`.
# At this point, the objective function value is :math:`-2.5`
#
# N.B.: OpenTURNS requires equality and inequality constraints to be stated as
# :math:`g(x) = 0` and :math:`h(x) \geq 0`, respectively. Thus the inequalities
# above will have to be restated to match this requirement:
#
# .. math::
#    \begin{array}{l}
#    -(x_1 - \frac{1}{2})^2 - (x_2 - \frac{1}{2})^2 + \frac{1}{4} \geq 0\\
#    -x_0 + x_1 \geq 0 \\
#    -x_0 - x_2 - x_3 + 2 \geq 0\\
#    \end{array}

# %%
# Definition of objective function
objectiveFunction = ot.SymbolicFunction(
    ['x0', 'x1', 'x2', 'x3'], ['-x0 -x1 -x2'])

# Definition of variables bounds
bounds = ot.Interval([0, 0, 0, 0], [1, 1e99, 1e99, 5], [
                     True, True, True, True], [True, False, False, True])

# Definition of constraints
# Constraints in OpenTURNS are defined as g(x) = 0 and h(x) >= 0
#    No equality constraint -> nothing to do
#    Inequality constraints:
h = ot.SymbolicFunction(['x0', 'x1', 'x2', 'x3'], [
                        '-(x1-0.5)^2 - (x2-0.5)^2 + 0.25',
                        'x1 - x0', '-x0 - x2 - x3 + 2'])

# Definition of variables types
CONTINUOUS = ot.OptimizationProblemImplementation.CONTINUOUS
BINARY = ot.OptimizationProblemImplementation.BINARY
INTEGER = ot.OptimizationProblemImplementation.INTEGER
variablesType = [BINARY, CONTINUOUS, CONTINUOUS, INTEGER]

# Setting up Bonmin problem
problem = ot.OptimizationProblem(objectiveFunction)
problem.setBounds(bounds)
problem.setVariablesType(variablesType)
problem.setInequalityConstraint(h)

bonminAlgorithm = ot.Bonmin(problem, 'B-BB')
bonminAlgorithm.setMaximumEvaluationNumber(10000)
bonminAlgorithm.setMaximumIterationNumber(1000)
bonminAlgorithm.setStartingPoint([0, 0, 0, 0])

ot.ResourceMap.AddAsString('Bonmin-mu_oracle', 'loqo')
ot.ResourceMap.AddAsScalar('Bonmin-bonmin.time_limit', 5)

# %%
# Running the solver
bonminAlgorithm.run()

# Retrieving the results
result = bonminAlgorithm.getResult()
print(" -- Optimal point = " + str(result.getOptimalPoint()))
print(" -- Optimal value = " + str(result.getOptimalValue()))
print(" -- Evaluation number = " + str(result.getInputSample().getSize()))

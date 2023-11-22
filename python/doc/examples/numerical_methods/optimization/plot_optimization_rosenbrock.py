"""
Quick start guide to optimization
=================================
"""
# %%
#
# In this example, we perform the optimization of the Rosenbrock test function.
#
# Let :math:`a, b\in\mathbb{R}` be parameters. The Rosenbrock function is defined by
#
# .. math::
#    f(x_1, x_2) = (a-x_1)^2 + b(x_2 - x_1^2)^2
#
#
# for any :math:`\mathbf{x}\in\mathbb{R}^2`.
# This function is often used with :math:`a=1` and :math:`b=100`. In this case, the function has a single global minimum at:
#
# .. math::
#    \mathbf{x}^\star = (1, 1)^T.
#
#
# This function has a nonlinear least squares structure.
#
# *References*
#
# * Rosenbrock, H.H. (1960). "An automatic method for finding the greatest or least value of a function". The Computer Journal. 3 (3): 175–184.

# %%
# Definition of the function
# --------------------------

# %%
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt

ot.Log.Show(ot.Log.NONE)

# %%
rosenbrock = ot.SymbolicFunction(["x1", "x2"], ["(1-x1)^2+100*(x2-x1^2)^2"])

# %%
x0 = [-1.0, 1.0]

# %%
xexact = ot.Point([1.0, 1.0])

# %%
lowerbound = [-2.0, -2.0]
upperbound = [2.0, 2.0]

# %%
# Plot the iso-values of the objective function
# ---------------------------------------------

# %%
rosenbrock = ot.MemoizeFunction(rosenbrock)

# %%
graph = rosenbrock.draw(lowerbound, upperbound, [100] * 2)
graph.setTitle("Rosenbrock function")
view = viewer.View(graph)

# %%
# We see that the minimum is on the top right of the picture and the starting point is on the top left of the picture.
# Since the function has a long valley following the curve :math:`x_2 - x^2=0`, the algorithm generally have to follow the bottom of the valley.

# %%
# Create and solve the optimization problem
# -----------------------------------------

# %%
problem = ot.OptimizationProblem(rosenbrock)

# %%
algo = ot.Cobyla(problem)
algo.setMaximumRelativeError(1.0e-1)  # on x
algo.setMaximumCallsNumber(50000)
algo.setStartingPoint(x0)
algo.run()

# %%
result = algo.getResult()

# %%
xoptim = result.getOptimalPoint()
xoptim

# %%
delta = xexact - xoptim
absoluteError = delta.norm()
absoluteError

# %%
# We see that the algorithm found an accurate approximation of the solution.

# %%
result.getOptimalValue()  # f(x*)

# %%
result.getCallsNumber()

# %%
graph = rosenbrock.draw(lowerbound, upperbound, [100] * 2)
cloud = ot.Cloud(ot.Sample([x0, xoptim]))
cloud.setColor("black")
cloud.setPointStyle("bullet")
graph.add(cloud)
graph.setTitle("Rosenbrock function")
view = viewer.View(graph)

# %%
# We see that the algorithm had to start from the top left of the banana and go to the top right.

# %%
graph = result.drawOptimalValueHistory()
view = viewer.View(graph)

# %%
# The function value history make the path of the algorithm clear.
# In the first step, the algorithm went in the valley, which made the function value decrease rapidly.
# Once there, the algorithm had to follow the bottom of the valley so that the function decreased but slowly.
# In the final steps, the algorithm found the neighbourhood of the minimum so that the local convergence could take place.

# %%
# In order to see where the function was evaluated, we use the `getInputSample` method.

# %%
inputSample = result.getInputSample()

# %%
graph = rosenbrock.draw(lowerbound, upperbound, [100] * 2)
graph.setTitle("Rosenbrock function solved with Cobyla")
cloud = ot.Cloud(inputSample)
graph.add(cloud)
view = viewer.View(graph)

# %%
# We see that the algorithm made lots of evaluations in the bottom of the valley before getting in the neighbourhood of the minimum.

# %%
# Solving the problem with NLopt
# ------------------------------
#
# We see that the `Cobyla` algorithm required lots of function evaluations.
# This is why we now use the `NLopt` class with the LBFGS algorithm.
# However, the algorithm may use input points which are far away from the input domain we used so far.
# This is why we had bounds to the problem, so that the algorithm never goes to far away from the valley.

# %%
bounds = ot.Interval(lowerbound, upperbound)

# %%
problem = ot.OptimizationProblem(rosenbrock)
problem.setBounds(bounds)

# %%
algo = ot.NLopt(problem, "LD_LBFGS")
algo.setStartingPoint(x0)
algo.run()

# %%
result = algo.getResult()

# %%
xoptim = result.getOptimalPoint()
xoptim

# %%
delta = xexact - xoptim
absoluteError = delta.norm()
absoluteError

# %%
# We see that the algorithm found an extremely accurate approximation of the solution.

# %%
result.getOptimalValue()  # f(x*)

# %%
result.getCallsNumber()

# %%
# This number of iterations is much less than the previous experiment.

# %%
inputSample = result.getInputSample()

# %%
graph = rosenbrock.draw(lowerbound, upperbound, [100] * 2)
graph.setTitle("Rosenbrock function solved with NLopt/LD_LBFGS")
cloud = ot.Cloud(inputSample)
graph.add(cloud)
view = viewer.View(graph)

plt.show()

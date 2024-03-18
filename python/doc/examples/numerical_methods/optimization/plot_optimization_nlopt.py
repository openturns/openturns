"""
Optimization using NLopt
========================
"""

# %%
# In this example we are going to explore optimization using the interface to the `NLopt <https://nlopt.readthedocs.io/en/latest/>`_ library.

# %%
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt

ot.Log.Show(ot.Log.NONE)

# %%
# List available algorithms
for algo in ot.NLopt.GetAlgorithmNames():
    print(algo)

# %%
# More details on NLopt algorithms are available `here <https://nlopt.readthedocs.io/en/latest/NLopt_Algorithms/>`_ .

# %%
# The optimization algorithm is instantiated from the NLopt name
algo = ot.NLopt("LD_SLSQP")

# %%
# define the problem
objective = ot.SymbolicFunction(["x1", "x2"], ["100*(x2-x1^2)^2+(1-x1)^2"])
inequality_constraint = ot.SymbolicFunction(["x1", "x2"], ["x1-2*x2"])
dim = objective.getInputDimension()
bounds = ot.Interval([-3.0] * dim, [5.0] * dim)
problem = ot.OptimizationProblem(objective)
problem.setMinimization(True)
problem.setInequalityConstraint(inequality_constraint)
problem.setBounds(bounds)

# %%
# solve the problem
algo.setProblem(problem)
startingPoint = [0.0] * dim
algo.setStartingPoint(startingPoint)
algo.run()

# %%
# retrieve results
result = algo.getResult()
print("x^=", result.getOptimalPoint())

# %%
# draw optimal value history
graph = result.drawOptimalValueHistory()
view = viewer.View(graph)
plt.show()

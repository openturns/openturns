"""
Optimization using NLopt
========================
"""

# %%
# In this example we are going to explore optimization using OpenTURNS' `NLopt <https://nlopt.readthedocs.io/en/latest/>`_ interface.

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
import math as m

# %%
# List available algorithms
for algo in ot.NLopt.GetAlgorithmNames():
    print(algo)

# %%
# More details on NLopt algorithms are available `here <https://nlopt.readthedocs.io/en/latest/NLopt_Algorithms/>`_ .

# %%
# The optimization algorithm is instanciated from the NLopt name
algo = ot.NLopt('LD_SLSQP')

# %%
# define the problem
objective = ot.SymbolicFunction(['x1', 'x2'], ['100*(x2-x1^2)^2+(1-x1)^2'])
inequality_constraint = ot.SymbolicFunction(['x1', 'x2'], ['x1-2*x2'])
dim = objective.getInputDimension()
bounds = ot.Interval([-3.] * dim, [5.] * dim)
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
print('x^=', result.getOptimalPoint())

# %%
# draw optimal value history
graph = result.drawOptimalValueHistory()
view = viewer.View(graph)
if not viewer._noshow:
    plt.show()

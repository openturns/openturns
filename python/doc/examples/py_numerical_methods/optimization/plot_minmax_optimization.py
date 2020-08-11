"""
Mix/max search using optimization
=================================
"""
# %% 
#
# In this example we are going to evaluate the min and max values of the output variable of interest in a domain using an optimization algorithm.

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
import math as m

# %%
# Create the marginal distributions of the parameters
dist_E = ot.Beta(0.93, 2.27, 2.8e7, 4.8e7)
dist_F = ot.LogNormalMuSigma(30000, 9000, 15000).getDistribution()
dist_L = ot.Uniform(250, 260)
dist_I = ot.Beta(2.5, 1.5, 3.1e2, 4.5e2)
marginals = [dist_E, dist_F, dist_L, dist_I]
distribution = ot.ComposedDistribution(marginals)

# %%
# Define bounds
lowerBound = [marginal.computeQuantile(0.1)[0] for marginal in marginals]
upperBound = [marginal.computeQuantile(0.9)[0] for marginal in marginals]
bounds = ot.Interval(lowerBound, upperBound)

# %%
# Create the model
model = ot.SymbolicFunction(['E', 'F', 'L', 'I'], ['F*L^3/(3*E*I)'])

# %%
# Define the problems
minProblem = ot.OptimizationProblem(model)
minProblem.setBounds(bounds)

maxProblem = ot.OptimizationProblem(model)
maxProblem.setBounds(bounds)
maxProblem.setMinimization(False)

# %%
# Create a solver
solver = ot.TNC()
solver.setStartingPoint(distribution.getMean())

# %%
# Solve the problems
solver.setProblem(minProblem)
solver.run()
minResult = solver.getResult()
print('min: y=', minResult.getOptimalValue(), 'with x=', minResult.getOptimalPoint())

solver.setProblem(maxProblem)
solver.run()
maxResult = solver.getResult()
print('max: y=', maxResult.getOptimalValue(), 'with x=', maxResult.getOptimalPoint())

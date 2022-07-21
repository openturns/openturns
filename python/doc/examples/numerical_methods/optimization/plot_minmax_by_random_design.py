"""
Mix/max search and sensitivity from design
==========================================
"""
# %%
#
# In this example, we are going to evaluate the minimum and maximum values of the output variable of interest from a sample and to evaluate the gradient of the limit-state function defining the output variable of interest at a particular point.

# %%
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
import math as m
ot.Log.Show(ot.Log.NONE)

# %%
# Create the marginal distributions of the parameters.
dist_E = ot.Beta(0.93, 2.27, 2.8e7, 4.8e7)
dist_F = ot.LogNormalMuSigma(30000, 9000, 15000).getDistribution()
dist_L = ot.Uniform(250, 260)
dist_I = ot.Beta(2.5, 1.5, 3.1e2, 4.5e2)
marginals = [dist_E, dist_F, dist_L, dist_I]
distribution = ot.ComposedDistribution(marginals)

# %%
# Sample the inputs.
sampleX = distribution.getSample(100)

# %%
# Create the model.
model = ot.SymbolicFunction(['E', 'F', 'L', 'I'], ['F*L^3/(3*E*I)'])

# %%
# Evaluate the outputs.
sampleY = model(sampleX)

# %%
# Get minimum and maximum values of both inputs and output variables.
minY = sampleY.getMin()
minX = sampleX[sampleY.find(minY)]
print('min: y=', minY, ' with x=', minX)
maxY = sampleY.getMax()
maxX = sampleX[sampleY.find(maxY)]
print('max: y=', maxY, ' with x=', maxX)

# %%
# Get sensitivity at minimum input values.
model.gradient(minX)

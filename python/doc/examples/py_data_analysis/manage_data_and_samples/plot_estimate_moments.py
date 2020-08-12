"""
Estimate moments from sample
============================
"""
# %%
# In this example we are going to estimate statistical moments from a sample, eventually from an output variable of interest.

# %%
from __future__ import print_function
import openturns as ot

# %%
# Create a sample 

# model f
model = ot.SymbolicFunction(["x1", "x2"], ["x1^2+x2", "x2^2+x1"])

# input vector X
inputDist = ot.ComposedDistribution([ot.Normal()] * 2, ot.IndependentCopula(2))
inputDist.setDescription(['X1', 'X2'])
inputVector = ot.RandomVector(inputDist)

# output vector Y=f(X)
output = ot.CompositeRandomVector(model, inputVector)

# sample Y
size = 1000
sample = output.getSample(size)

# %%
# Estimate mean
sample.computeMean()

# %%
# Estimate standard deviation
sample.computeStandardDeviation()

# %%
# Estimate variance
sample.computeVariance()

# %%
# Estimate skewness
sample.computeSkewness()

# %%
# Estimate kurtosis
sample.computeKurtosis()

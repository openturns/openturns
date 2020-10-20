"""
Fit a distribution from an input sample
=======================================
"""
# %% 
#
# In this example we show how to use the `BuildDistribution` function to fit a distribution to an input sample. This function is used by the `FunctionalChaosAlgorithm` class when we want to create a polynomial chaos metamodel and we have a design of experiments which have been computed beforehand. In this case, we have to identify the distributions which best fit to the input sample in order to define the input probabilistic model. This is, in turn, used by in the polynomial chaos to create the orthogonal basis. 
#

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
ot.Log.Show(ot.Log.NONE)

# %%
# We first create the function `model`.

# %%
ot.RandomGenerator.SetSeed(0)
dimension = 2
input_names = ['x1', 'x2']
formulas = ['cos(x1 + x2)', '(x2 + 1) * exp(x1)']
model = ot.SymbolicFunction(input_names, formulas)

# %%
# Then we create a sample `x` and compute the corresponding output sample `y`.

# %%
distribution = ot.Normal(dimension)
samplesize = 1000
inputSample  = distribution.getSample(samplesize)
outputSample = model(inputSample)

# %%
# Create a functional chaos model. 
# First, we need to fit a distribution on the input sample. We can do this automatically with the Lilliefors test.

# %%
ot.ResourceMap.SetAsUnsignedInteger("FittingTest-LillieforsMaximumSamplingSize", 100)

# %%
distribution = ot.FunctionalChaosAlgorithm.BuildDistribution(inputSample)

# %%
for i in range(dimension):
    marginal = distribution.getMarginal(i)
    marginalname = marginal.getImplementation().getClassName()
    print("Marginal #",i,":",marginalname)
distribution.getCopula()

# %%
# The previous call to `BuildDistribution` is what is done internally by the following constructor of `FunctionalChaosAlgorithm`.

# %%
algo = ot.FunctionalChaosAlgorithm(inputSample, outputSample)

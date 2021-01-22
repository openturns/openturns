"""
Randomize the lines of a Sample
===============================
"""
# %%
import openturns as ot

# %%
# In this short example we present a simple way to mix the lines of a sample 
# thanks to the :class:`~openturns.KPermutationsDistribution` class.

# %%
# We first define a small sample of size :math:`N` based on a standard unit gaussian distribution.
distribution = ot.Normal()
N = 5
sample = distribution.getSample(N)

# %%
# We print the sample :
print(sample)

# %%
# A new set of randomly mixed indices is a realization of a permutation of N elements amongst N :
#
mixingDistribution = ot.KPermutationsDistribution(N, N)
newIndices = mixingDistribution.getSample(1)[0,:]

# %%
# The new indices will be these ones :
print("New indices : ", newIndices)

# %%
# Eventually the randomized sample is
print( sample[[int(i) for i in newIndices]])

"""
Combinatorial generators
========================
"""
# %%
# In this example we are going to expose the various desing of experiments that allow to generate all the integer collections satisfying a given combinatorial constraint:
#
# - The Tuples generator, which allows one to generate all the elements of a Cartesian product :math:`E=\{0,\dots,n_0-1\}\times\dots\times\{0,\dots,n_{d-1}-1\}`.
#
#   The total number of generated points is :math:`N=\prod_{k=0}^{d-1}n_k`.
#
# - The K-permutations generator, which allows one to generate all the injective functions from :math:`\{0,\dots,k-1\}` into :math:`\{0,\dots,n-1\}`
#
#   The total number of generated points is :math:`N=\dfrac{n!}{(n-k)!}`.
#
# - The Combinations generator, which allows one to generate all the subsets of size :math:`k` of :math:`\{0,\dots,n-1\}`
#
#   The total number of generated points is :math:`N=\dfrac{n!}{k!(n-k)!}`.
#

# %%
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
import math as m
ot.Log.Show(ot.Log.NONE)

# %%
# Tuples
# ------
experiment = ot.Tuples([2, 3, 5])
print(experiment.generate())

# %%
# K-permutations
# --------------
experiment = ot.KPermutations(3, 4)
print(experiment.generate())

# %%
# Combinations
# ------------
experiment = ot.Combinations(4, 6)
print(experiment.generate())

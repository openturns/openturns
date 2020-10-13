"""
Create a random mixture of distributions
========================================
"""
# %%
# In this example we are going to build an affine combination of input random variables.
#
# .. math:: 
#    Y = 2 + 5 X_1 + X_2
#   
# where:
#
# - :math:`X1 \sim \mathcal{E}(\lambda=1.5)`
# - :math:`X2 \sim \mathcal{N}(\mu=4, \sigma=1)`
#
# This notion is different from the Mixture where the combination is made on the probability density functions and not on the univariate random variable.

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
ot.Log.Show(ot.Log.NONE)

# %%
# create the distributions associated to the input random variables
X1 = ot.Exponential(1.5)
X2 = ot.Normal(4.0, 1.0)

# %%
# offset
a0 = 2.0

# %%
# Create the weights
weight = [5.0, 1.0]

# %%
# create the affine combination
distribution = ot.RandomMixture([X1, X2], weight, a0)
distribution

# %%
# ask its mean
distribution.getMean()

# %%
# ask its variance
distribution.getCovariance()[0, 0]

# %%
# ask the 90% quantile
distribution.computeQuantile(0.9)

# %%
# ask its probability to exceeds 3
distribution.computeSurvivalFunction(3.0)

# %%
# draw PDF
graph = distribution.drawPDF()
view = viewer.View(graph)

# %%
# draw PDF
graph = distribution.drawCDF()
view = viewer.View(graph)
plt.show()


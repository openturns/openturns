"""
Create a discrete random mixture
================================
"""
# %% 

# %%
# In this example we are going to build the distribution of the value of the sum of 20 dice rolls.
#
# .. math:: 
#    Y = \sum_{i=1}^{20} X_i
#   
# where :math:`X_i \tilde{} U(1,2,3,4,5,6)`
#

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt

# %%
# create the distribution associated to the dice roll
X = ot.UserDefined([[i] for i in range(1,7)])

# %%
# Roll the dice a few times
X.getSample(10)

# %%
N = 20

# %%
# Create the collection of identically distributed Xi
coll = [X] * N

# %%
# Create the weights
weight = [1.0] * N

# %%
# create the affine combination
distribution = ot.RandomMixture(coll, weight)

# %%
# probability to exceed a sum of 100 after 20 dice rolls
distribution.computeComplementaryCDF(100)

# %%
# draw PDF
graph = distribution.drawPDF()
view = viewer.View(graph)

# %%
# draw CDF
graph = distribution.drawCDF()
view = viewer.View(graph)
plt.show()

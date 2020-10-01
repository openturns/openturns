"""
Create a maximum entropy statistics distribution
================================================
"""
# %%
# In this example we are going to build maximum entropy statistics distribution, which yields ordered realizations:
#
# .. math::
#    X_1 \leq \dots \leq X_n
#

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt

# %%
# create a collection of distribution
coll = [ot.Beta(1.5, 1.7, 0.0, 1.0),  ot.Beta(2.0, 2.3, 0.5, 1.2)]

# %%
# create the distribution
distribution = ot.MaximumEntropyOrderStatisticsDistribution(coll)
print(distribution)

# %%
# draw a sample (ordered!)
distribution.getSample(10)

# %%
# draw PDF
graph = distribution.drawPDF()
view = viewer.View(graph)
plt.show()

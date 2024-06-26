"""
Create the distribution of the maximum of independent distributions
===================================================================
"""
# %%
# In this example we are going to build the distribution of the maximum of independent distributions.

# %%
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt

ot.Log.Show(ot.Log.NONE)

# %%
# Create a collection of distributions
distribution1 = ot.Normal()
distribution2 = ot.Uniform(-1.0, 2.0)
distColl = [distribution1, distribution2]

# %%
# Create the distribution
distribution = ot.MaximumDistribution(distColl)
print(distribution)

# %%
# Draw PDF
graph = distribution.drawPDF()
view = viewer.View(graph)
plt.show()

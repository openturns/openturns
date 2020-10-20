"""
Fit a parametric copula
=======================
"""

# %%
# In this example we are going to estimate the parameters of a gaussian copula from a sample.

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
ot.Log.Show(ot.Log.NONE)

# %%
# Create data
R = ot.CorrelationMatrix(2)
R[1, 0] = 0.4
copula = ot.NormalCopula(R)
sample = copula.getSample(500)

# %%
# Estimate a normal copula
distribution = ot.NormalCopulaFactory().build(sample)
print(distribution)

# %%
# The estimated parameters
distribution.getParameter()

# %%
# Draw fitted distribution
graph = distribution.drawPDF()
view = viewer.View(graph)
plt.show()

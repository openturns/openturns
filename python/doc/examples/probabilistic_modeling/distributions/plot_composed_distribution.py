"""
Create a composed distribution
==============================
"""
# %%
# In this example we are going to create a multidimensional distribution described by its marginal distributions and optionally its dependence structure (a particular copula).

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
ot.Log.Show(ot.Log.NONE)

# %%
# create the marginals
marginals = [ot.Normal(), ot.Gumbel()]

# %%
# create the multivariate distribution, assume no dependency structure
distribution = ot.ComposedDistribution(marginals)

# %%
# draw PDF
graph = distribution.drawPDF()
view = viewer.View(graph)

# %%
# create the copula which specifies the dependency structure
R = ot.CorrelationMatrix(2)
R[0, 1] = 0.3
copula = ot.NormalCopula(R)
copula

# %%
# create the multivariate distribution with the desired copula
distribution = ot.ComposedDistribution(marginals, copula)

# %%
# draw PDF
graph = distribution.drawPDF()
view = viewer.View(graph)
plt.show()

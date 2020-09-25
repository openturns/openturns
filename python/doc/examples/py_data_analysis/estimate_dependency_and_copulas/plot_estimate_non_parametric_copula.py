"""
Fit a non parametric copula
===========================
"""

# %%
# In this example we are going to estimate a normal copula from a sample using non parametric representations.

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt

# %%
# Create data
R = ot.CorrelationMatrix(2)
R[1, 0] = 0.4
copula = ot.NormalCopula(R)
sample = copula.getSample(30)

# %%
# Estimate a normal copula using BernsteinCopulaFactory
distribution = ot.BernsteinCopulaFactory().build(sample)

# %%
# Draw fitted distribution
graph = distribution.drawPDF()
view = viewer.View(graph)

# %%
# Estimate a normal copula using KernelSmoothing
distribution = ot.KernelSmoothing().build(sample).getCopula()
graph = distribution.drawPDF()
view = viewer.View(graph)
if not viewer._noshow:
    plt.show()

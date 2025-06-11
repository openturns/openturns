"""
Fit a non parametric copula
===========================
"""

# %%
# In this example we are going to estimate a Normal copula from a sample using non parametric representations.

# %%
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pyplot as plt


# %%
# Create data
R = ot.CorrelationMatrix(2)
R[1, 0] = 0.4
copula = ot.NormalCopula(R)
sample = copula.getSample(30)

# %%
# Estimate a Normal copula using :class:`~openturns.BernsteinCopulaFactory`
distribution = ot.BernsteinCopulaFactory().build(sample)

# %%
# Draw fitted distribution
graph = distribution.drawPDF()
view = viewer.View(graph)

# %%
# Estimate a Normal copula using :class:`~openturns.KernelSmoothing`
distribution = ot.KernelSmoothing().build(sample).getCopula()
graph = distribution.drawPDF()
view = viewer.View(graph)
plt.show()

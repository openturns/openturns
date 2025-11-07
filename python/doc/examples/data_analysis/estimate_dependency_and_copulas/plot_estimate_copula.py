"""
Fit a parametric copula
=======================
"""

# %%
# In this example we are going to estimate the parameters of a Gaussian copula from a sample.

# %%
import openturns as ot
import openturns.viewer as otv
from matplotlib import pyplot as plt


# %%
# Create data
R = ot.CorrelationMatrix(2)
R[1, 0] = 0.4
copula = ot.NormalCopula(R)
sample = copula.getSample(500)

# %%
# Estimate a Normal copula
distribution = ot.NormalCopulaFactory().build(sample)
print(distribution)

# %%
# The estimated parameters
distribution.getParameter()

# %%
# Draw fitted distribution
graph = distribution.drawPDF()
view = otv.View(graph)
plt.show()

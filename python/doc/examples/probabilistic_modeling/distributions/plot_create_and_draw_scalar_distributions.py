"""
Create and draw scalar distributions
====================================
"""
# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
ot.Log.Show(ot.Log.NONE)


# %%
# A continuous distribution
# -------------------------
#
# We build a normal distribution with parameters :
#
# .. math::
#     \mu = 2.2, \sigma = 0.6
#
distribution = ot.Normal(2.2, 0.6)
print(distribution)

# %%
# We can draw a sample following this distribution with the `getSample` method :
size = 10
sample = distribution.getSample(size)
print(sample)

# %%
# We draw its PDF and CDF :
graphPDF = distribution.drawPDF()
graphPDF.setTitle(
    r"PDF of a normal distribution with parameters $\mu = 2.2$ and $\sigma = 0.6$")
view = viewer.View(graphPDF)

# %%
graphCDF = distribution.drawCDF()
graphCDF.setTitle(
    r"CDF of a normal distribution with parameters $\mu = 2.2$ and $\sigma = 0.6$")
view = viewer.View(graphCDF)


# %%
# A discrete distribution
# -----------------------
#
# We define a geometric distribution with parameter :math:`p = 0.7`.
p = 0.7
distribution = ot.Geometric(p)
print(distribution)

# %%
# We draw a sample of it :
size = 10
sample = distribution.getSample(size)
print(sample)

# %%
# We draw its PDF and its CDF :
graphPDF = distribution.drawPDF()
graphPDF.setTitle(r"PDF of a geometric distribution with parameter $p = 0.7$")
view = viewer.View(graphPDF)

# %%
graphCDF = distribution.drawCDF()
graphCDF.setTitle(r"CDF of a geometric distribution with parameter $p = 0.7$")
view = viewer.View(graphCDF)

# %%
# Conclusion
# ----------
#
# The two previous examples look very similar despite their continuous and discrete nature.
# In the library there is no distinction between continuous and discrete distributions.

# %%
# Display all figures
plt.show()

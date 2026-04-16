"""
Draw the empirical CDF
======================
"""

# %%
# In this example we are going to draw the empirical CDF of an unidimensional sample.

# %%
import openturns as ot
import openturns.viewer as otv

# %%
# Then create a sample from a Gaussian distribution.

# %%
size = 100
normal = ot.Normal(1)
sample = normal.getSample(size)

# %%
# We draw the empirical CDF based on the :class:`~openturns.FiniteDiscreteDistribution` distribution.
# By default, the `drawCDF` method requires no input argument.

# %%
distribution = ot.FiniteDiscreteDistribution(sample)
graph = distribution.drawCDF()
view = otv.View(graph)

# %%
# If required, we can specify the interval that we want to draw.
# In the following example, these bounds are computed from the minimum and the maximum of the sample.

# %%
xmin = sample.getMin()[0] - 2.0
xmax = sample.getMax()[0] + 2.0
graph = ot.FiniteDiscreteDistribution(sample).drawCDF(xmin, xmax)
view = otv.View(graph)

# %%
otv.View.ShowAll()

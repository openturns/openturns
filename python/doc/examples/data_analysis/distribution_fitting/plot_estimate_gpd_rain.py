"""
Estimate a GPD on the daily rainfall data
=========================================
"""
# %%
# In this example, we illustrate various techniques of extreme value modeling applied
# to the daily rainfall accumulations in south-west England, over the period 1914-1962.
# Readers should refer to [coles2001]_ to get more details.
#
# We illustrate techniques to:
#
# - estimate a stationary and a non stationary GEV,
# - estimate a return level,
#
# using:
#
# - the log-likelihood function,
# - the profile log-likelihood function.
#
import openturns as ot
import openturns.viewer as otv
from openturns.usecases import coles

# %%
# First, we load the Rain dataset. We start by looking at it through time.
data = coles.Coles().rain
print(data[:10])

# %%
# Draw the mean residual life plot
# The curve becomes linear from a threshold :math:`u_s=60`
factory = ot.GeneralizedParetoFactory()
graph = factory.drawMeanResidualLife(data)
view = otv.View(graph)

# %%
otv.View.ShowAll()

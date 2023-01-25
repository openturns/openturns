"""
Estimate a GEV on the race time data
====================================
"""
# %%
# In this example we are going to estimate the parameters of a generalized extreme value distribution
# on the women 1500m annual race time data from [coles2001]_:
#
# - First with the maximum the log-likehood estimator from independent observations (paragraph 3.4.1)

# %%
# Load the dataset
import openturns as ot
import openturns.viewer as otv
from openturns.usecases import coles
data = coles.Coles().racetime
print(data[:5])

# %%
# Plot the 10 largest sea levels per year
graph = ot.Graph('Fastest annual women 1500m race time', 'Year', 'Race time (s)', True, '')
cloud = ot.Cloud(data)
cloud.setPointStyle('fcircle')
# cloud.setPointSize(1.0)
graph.add(cloud)
graph.setIntegerXTick(True)
view = otv.View(graph)

# %%
# Select the race time column, multiply by -1 to transform the min into max
sample = -1.0 * data[:, 1]

# %%
# Estimate the parameters of the GEV by maximizing the log-likehood
# and compute the parameter distribution
factory = ot.GeneralizedExtremeValueFactory()
result = factory.buildMethodOfLikelihoodMaximizationEstimator(sample)

# %%
# Print the estimated parameter values
estimate = result.getDistribution()
desc = estimate.getParameterDescription()
p = estimate.getParameter()
print(", ".join([f"{param}: {value:.3f}" for param, value in zip(desc, p)]))

# %%
otv.View.ShowAll()

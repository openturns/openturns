"""
Estimate a GEV on the race time data
====================================
"""
# %%
# In this example we are going to estimate the parameters of a generalized extreme value distribution
# on the women 1500m annual race time data from [coles2001]_ with different methods:
#
# - the maximum likelihood estimation
# - the profile likelihood estimation
# - the estimation of return level from both maximum likelihood and profile likelihood

# %%
# Load the dataset
import openturns as ot
import openturns.viewer as otv
import openturns.experimental as otexp
from openturns.usecases import coles

data = coles.Coles().racetime
print(data[:5])

# %%
# Plot the 10 largest sea levels per year
graph = ot.Graph(
    "Fastest annual women 1500m race time", "Year", "Race time (s)", True, ""
)
cloud = ot.Cloud(data)
cloud.setPointStyle("fcircle")
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
result1 = factory.buildMethodOfLikelihoodMaximizationEstimator(sample)

# %%
# Print the estimated parameter values
estimate = result1.getDistribution()
desc = estimate.getParameterDescription()
p = estimate.getParameter()
print(", ".join([f"{param}: {value:.3f}" for param, value in zip(desc, p)]))

# %%
# Validate the inference result thanks to some diagnostic plots
validation = otexp.GeneralizedExtremeValueValidation(result1, sample)
graph = validation.drawDiagnosticPlot()
view = otv.View(graph)

# %%
# Now estimate the parameters with the profile likelihood
result2 = factory.buildMethodOfProfileLikelihoodMaximizationEstimator(sample)

# %%
# We can see the confidence interval of :math:`\xi` is a bit smaller with this method
result2.setConfidenceLevel(0.95)
print(result2.getParameterConfidenceInterval())

# %%
# We can also plot the profile likelihood and the confidence interval
view = otv.View(result2.drawProfileLikelihoodFunction())

# %%
# Estimate the 10 and 100 years return levels from the MLE estimator
zm10 = factory.buildReturnLevelEstimator(result1, 10.0)
return_level10 = zm10.getMean()
print(f"10 years return level={return_level10}")
return_level_ci10 = zm10.computeBilateralConfidenceInterval(0.95)
print(f"CI={return_level_ci10}")

zm100 = factory.buildReturnLevelEstimator(result1, 100.0)
return_level100 = zm100.getMean()
print(f"100 years return level={return_level100}")
return_level_ci100 = zm100.computeBilateralConfidenceInterval(0.95)
print(f"CI={return_level_ci100}")

# %%
# Now estimate the 10 years return level with profile likelihood
result_rl10_prof = factory.buildReturnLevelProfileLikelihoodEstimator(sample, 10.0)
zm = result_rl10_prof.getParameter()
print(f"10 years return level (profile)={zm}")

# %%
# We can see the confidence interval of :math:`z_m` is a bit smaller with this method
result_rl10_prof.setConfidenceLevel(0.95)
return_level_ci10 = result_rl10_prof.getParameterConfidenceInterval()
print(f"CI={return_level_ci10}")

# %%
# We can also plot the profile likelihood and the confidence interval
view = otv.View(result_rl10_prof.drawProfileLikelihoodFunction())

# %%
otv.View.ShowAll()

"""
Estimate a GEV on the Venice sea levels data
============================================
"""
# %%
# In this example we are going to estimate the parameters of a generalized extreme value distribution
# on the Venice sea levels data from [coles2001]_ with different methods:
#
# - the maximum likelihood estimation
# - the profile likelihood estimation
# - the estimation of return level from both maximum likelihood and profile likelihood
# - the R maxima estimator (see [coles2001]_ paragraph 3.5.3)

# %%
# Load the Venice dataset of 10 highest sea levels per year
import openturns as ot
import openturns.viewer as otv
import openturns.experimental as otexp
from openturns.usecases import coles

data = coles.Coles().venice
print(data[:5])

# %%
# Plot the 10 largest sea levels per year
graph = ot.Graph("Venice sea level", "year", "Sea level (cm)", True, "")
for r in range(10):
    cloud = ot.Cloud(data[:, [0, 1 + r]])
    graph.add(cloud)
graph.setIntegerXTick(True)
view = otv.View(graph)

# %%
# First work on the first column after the year column to retain only one max value per year
sample = data[:, 1]

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
# Inspect the estimated Gaussian parameter distribution
parameterEstimate = result1.getParameterDistribution()
print(parameterEstimate)

# %%
# Inspect the covariance matrix
V = parameterEstimate.getCovariance()
print(V)

# %%
# Inspect the standard deviation
stddev = parameterEstimate.getStandardDeviation()
print(stddev)

# %%
# Extract parameters confidence intervals
for i in range(3):
    ci = parameterEstimate.getMarginal(i).computeBilateralConfidenceInterval(0.95)
    print(desc[i] + ":", ci)

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
# Now drop the year column to keep only the maxima values
sample_rmax = data[:, 1:]
print(sample_rmax[:5])

# %%
# Estimate the parameters of the GEV from the r largest observations
# for different values of r
factory = ot.GeneralizedExtremeValueFactory()
r_candidate = [1, 5, 10]
for r in r_candidate:
    estimate = factory.buildRMaxima(sample_rmax, r)
    desc = estimate.getParameterDescription()
    p = estimate.getParameter()
    pretty_p = ", ".join([f"{param}: {value:.3f}" for param, value in zip(desc, p)])
    print(f"r={r:2} {pretty_p}")

# %%
# It is possible to retrieve the value of r which maximizes the likelihood
best_r, llh = factory.buildBestRMaxima(sample_rmax, r_candidate)
print(best_r)

# %%
# We can also get the associated likelihood value
for i in range(3):
    print(f"r={r_candidate[i]} likelihood={llh[i]}")

# %%
# We can also get the associated parameter distribution
result5 = factory.buildRMaximaEstimator(sample_rmax, 1)
print(result5.getParameterDistribution())

# %%
otv.View.ShowAll()

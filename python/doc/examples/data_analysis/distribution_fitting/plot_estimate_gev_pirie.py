"""
Estimate a GEV on the Pirie sea levels data
===========================================
"""
# %%
# In this example we are going to estimate the parameters of a generalized extreme value distribution
# on sea levels data from [coles2001]_:
#
# - First with the maximum the log-likehood estimator from independent observations (paragraph 3.4.1)
# - Then with the maximum the log-likehood estimator with a non-stationary hypothesis (paragraph 6.3.1)

# %%
# Load the Port Pirie dataset of highest sea levels per year
import openturns as ot
import openturns.viewer as otv
from openturns.usecases import coles
data = coles.Coles().portpirie
print(data[:5])

# %%
# Select the sea levels column
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
# Now estimate the parameters with the profile likelihood
result2 = factory.buildMethodOfProfileLikelihoodMaximizationEstimator(sample)

# %%
# We can see the confidence interval of :math:`\xi` is a bit smaller with this method
result2.setConfidenceLevel(0.95)
print(result2.getXiConfidenceInterval())

# %%
# We can also plot the profile likelihood and the confidence interval
view = otv.View(result2.drawProfileLikelihood())

# %%
# Now formulate a non-stationary hypothesis for our parameters
# First we need the grid of time values (in years)
mesh = ot.Mesh(data[:, 0])

# %%
# Now define the basis for each parameter
# lets say mu is linear with time, the other parameters remaining constant
constant = ot.SymbolicFunction(['t'], ['1.0'])
basis_mu = ot.Basis([constant, ot.SymbolicFunction(['t'], ['t'])])  # linear trend
basis_sigma = ot.Basis([constant])  # stationary
basis_xi = ot.Basis([constant])  # stationary
basis_coll = [basis_mu, basis_sigma, basis_xi]

# %%
# We can now estimate the list of coefficients of the parametric basis for each parameters
# the first 2 values are the coefficients for the mu basis
result3 = factory.buildTimeVarying(sample, mesh, basis_coll)
p = result3.getOptimalParameter()
b_mu, a_mu = p[:2]
print(p)
print(f"mu(t) = {b_mu:.4f} + {a_mu:.4f} * t")

# %%
# We can draw the estimated mu parametric function
# Lets superpose the cloud of data values
graph = result3.drawParameterFunction(0)
cloud = ot.Cloud(data)
cloud.setColor('red')
graph.add(cloud)
view = otv.View(graph)

# %%
# The visual evidence of a trend is weak in the graph.
# The strength of this evidence can be assessed with the deviance statistics:
result4 = ot.HypothesisTest.LikelihoodRatioTest(result1.getLogLikelihood(), result3.getLogLikelihood(), 0.05)
print(f"hypothesis H0(stationary model) accepted={result4.getBinaryQualityMeasure()}")

# %%
# The deviance statistics :math:`D_p` is small compared to the Chi2 quantile
# it means that the linear trend component can only explain a small variation in the data
# and is not a genuine effect in the sea-level process but rather a chance feature in the observed data
print(f"Dp={result4.getStatistic():.2f}")
print(f"cAlpha={result4.getThreshold():.2f}")

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
otv.View.ShowAll()

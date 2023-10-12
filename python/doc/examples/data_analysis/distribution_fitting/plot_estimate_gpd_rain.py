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
sample = coles.Coles().rain
print(sample[:10])

# %%
# Draw the mean residual life plot
# The curve becomes linear from a threshold :math:`u_s=60`
factory = ot.GeneralizedParetoFactory()
graph = factory.drawMeanResidualLife(sample)
view = otv.View(graph)

# %%
# **Stationary GPD modeling via the log-likelihood function**
#
# We first assume that the dependence through time is negligible, so we first model the data as
# independent observations over the observation period.
# We estimate the parameters of the GPD distribution by maximizing the log-likelihood of the data for a given threshold u.
u = 30
result_LL = factory.buildMethodOfLikelihoodMaximizationEstimator(sample, u)

# %%
# We get the fitted GPD and its parameters of :math:`(\hat{\sigma}, \hat{\xi}, u)`.
fitted_GEV = result_LL.getDistribution()
desc = fitted_GEV.getParameterDescription()
param = fitted_GEV.getParameter()
print(", ".join([f"{p}: {value:.3f}" for p, value in zip(desc, param)]))
print("log-likelihood = ", result_LL.getLogLikelihood())

# %%
# We get the asymptotic distribution of the estimator :math:`(\hat{\mu}, \hat{\sigma}, \hat{\xi})`.
# In that case, the asymptotic distribution is normal.
parameterEstimate = result_LL.getParameterDistribution()
print("Asymptotic distribution of the estimator : ")
print(parameterEstimate)

# %%
# We get the covariance matrix and the standard deviation of :math:`(\hat{\sigma}, \hat{\xi}, \hat{\xi})`.
print("Cov matrix = \n", parameterEstimate.getCovariance())
print("Standard dev = ", parameterEstimate.getStandardDeviation())

# %%
# We get the marginal confidence intervals of order 0.95.
order = 0.95
for i in range(3):
    ci = parameterEstimate.getMarginal(i).computeBilateralConfidenceInterval(order)
    print(desc[i] + ":", ci)

# %%
otv.View.ShowAll()

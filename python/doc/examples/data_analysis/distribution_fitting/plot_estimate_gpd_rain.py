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
# Draw the parameter stability plots
# The perturbations appear small relative to sampling errors and a smaller threshold can be chosen :math:`u_s=60`.
u_range = ot.Interval(0.5, 50.0)
graph = factory.drawParameterThresholdStability(sample, u_range)
view = otv.View(graph, figure_kw={"figsize": (6.0, 6.0)})

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
for i in range(2):  # exclude u parameter (fixed)
    ci = parameterEstimate.getMarginal(i).computeBilateralConfidenceInterval(order)
    print(desc[i] + ":", ci)

# %%
# **Stationary GPD modeling via the profile log-likelihood function**
#
# Now, we use the profile log-likehood function rather than log-likehood function  to estimate the parameters of the GPD.
result_PLL = factory.buildMethodOfXiProfileLikelihoodEstimator(sample, u)

# %%
# The following graph allows one to get the profile log-likelihood plot.
# It also indicates the optimal value of :math:`\xi`, the maximum profile log-likelihood and
# the confidence interval for :math:`\xi` of order 0.95 (which is the default value).
order = 0.95
result_PLL.setConfidenceLevel(order)
view = otv.View(result_PLL.drawProfileLikelihoodFunction())

# %%
# We can get the numerical values of the confidence interval: it appears to be a bit smaller
# with the interval obtained from the profile log-likelihood function than with the log-likelihood
# function.
# Note that if the order requested is too high, the confidence interval might not be calculated because
# one of its bound is out of the definition domain of the log-likelihood function.
try:
    print("Confidence interval for xi = ", result_PLL.getParameterConfidenceInterval())
except Exception as ex:
    print(type(ex))
    pass

# %%
# **Return level estimate from the estimated stationary GPD**
#
# We estimate the :math:`m`-block return level :math:`z_m`: it is computed as a particular quantile of the
# GPD model estimated using the log-likelihood function. We just have to use the maximum log-likelihood
# estimator built in the previous section.
#
# As the data are daily records, each block corresponds to one day: the 10-year return level
# corresponds to :math:`m=10*365` and the 100-year return level corresponds to :math:`m=100*365`.
#
# The method also provides the asymptotic distribution of the estimator :math:`\hat{z}_m`.
zm_10 = factory.buildReturnLevelEstimator(result_LL, 10.0 * 365, sample)
return_level_10 = zm_10.getMean()
print("Maximum log-likelihood function : ")
print(f"10-year return level = {return_level_10}")
return_level_ci10 = zm_10.computeBilateralConfidenceInterval(0.95)
print(f"CI = {return_level_ci10}")

zm_100 = factory.buildReturnLevelEstimator(result_LL, 100.0 * 365, sample)
return_level_100 = zm_100.getMean()
print(f"100-year return level = {return_level_100}")
return_level_ci100 = zm_100.computeBilateralConfidenceInterval(0.95)
print(f"CI = {return_level_ci100}")

# %%
# **Return level estimate via the profile log-likelihood function of a stationary GPD**
#
# We can estimate the :math:`m`-block return level :math:`z_m` directly from the data using the profile
# likelihood with respect to :math:`z_m`.
result_zm_100_PLL = factory.buildReturnLevelProfileLikelihoodEstimator(sample, u, 100.0 * 365)
zm_100_PLL = result_zm_100_PLL.getParameter()
print(f"10-year return level (profile) = {zm_100_PLL}")

# %%
# We can get the confidence interval of :math:`z_m`: once more, it appears to be a bit smaller
# than the interval obtained from the log-likelihood function.
result_zm_100_PLL.setConfidenceLevel(0.95)
return_level_ci100 = result_zm_100_PLL.getParameterConfidenceInterval()
print("Maximum profile log-likelihood function : ")
print(f"CI={return_level_ci100}")

# %%
# We can also plot the profile log-likelihood function and get the confidence interval, the optimal value
# of :math:`z_m` and its confidence interval.
view = otv.View(result_zm_100_PLL.drawProfileLikelihoodFunction())

# %%
otv.View.ShowAll()

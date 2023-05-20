"""
Estimate a GEV on the Venice sea-levels data
============================================
"""
# %%
# In this example, we illustrate various techniques of extreme value modeling applied
# to the annual maximum sea-levels recorded in Venice over the period 1931-1981.
# Readers should refer to [coles2001]_ to get more details.
#
# We illustrate techniques to:
#
# - estimate a stationary GEV, using the :math:`r`-largest annual sea-levels for :math:`r\geq 1`,
#
# using:
#
# - the log-likelihood function,
# - the profile log-likelihood function.
#
# First, we load the 10 largest annual sea-levels in Venice. We start by looking at them
# through time. Note that for the year 1935, only the largest 6 observations are available.
# For simplicity of the example, we removed that year from the data but it could be
# used for all the analyses beased on the largest :math:`r` annual sea-levels for :math:`r \leq 6`.
import openturns as ot
import openturns.viewer as otv
import openturns.experimental as otexp
from openturns.usecases import coles

data = coles.Coles().venice

# %%
# The column :math:`i` contains the largest :math:`i` annual sea-levels.
print(data[:5])
graph = ot.Graph("Annual maximum sea-levels in Venice", "year", "level (cm)", True, "")
for r in range(10):
    cloud = ot.Cloud(data[:, [0, 1 + r]])
    graph.add(cloud)
graph.setIntegerXTick(True)
view = otv.View(graph)

# %%
# **Stationary GEV modeling from the annual maximum sea-levels**
#
# We first assume that the dependence through time is negligible, so we first model the data as
# independent observations over the observation period. We estimate the parameters of the
# GEV distribution by maximizing the log-likelihood of the data.
# We select the first column of the data.
sample = data[:, 1]

# %%
# Estimate the parameters of the GEV by maximizing the log-likehood
# and compute the parameter distribution
factory = ot.GeneralizedExtremeValueFactory()
result_LL_max = factory.buildMethodOfLikelihoodMaximizationEstimator(sample)

# %%
# We get the fitted GEV and its parameters of :math:`(\hat{\mu}, \hat{\sigma}, \hat{\xi})`.
fitted_GEV = result_LL_max.getDistribution()
desc = fitted_GEV.getParameterDescription()
param = fitted_GEV.getParameter()
print(", ".join([f"{p}: {value:.3f}" for p, value in zip(desc, param)]))

# %%
# We get the asymptotic distribution of the estimator :math:`(\hat{\mu}, \hat{\sigma}, \hat{\xi})`.
# In that case, the asymptotic distribution is normal.
parameterEstimate = result_LL_max.getParameterDistribution()
print('Asymptotic distribution of the estimator : ')
print(parameterEstimate)

# %%
# We get the covariance matrix  and the standard deviation of :math:`(\hat{\mu}, \hat{\sigma}, \hat{\xi})`.
print('Cov matrix = ', parameterEstimate.getCovariance())
print('Standard dev = ', parameterEstimate.getStandardDeviation())

# %%
# We get the marginal confidence intervals of order 0.95.
order = 0.95
for i in range(3):
    ci = parameterEstimate.getMarginal(i).computeBilateralConfidenceInterval(order)
    print(desc[i] + ":", ci)

# %%
# At last, we can validate the inference result thanks the 4 usual diagnostic plots.
validation = otexp.GeneralizedExtremeValueValidation(result_LL_max, sample)
graph = validation.drawDiagnosticPlot()
view = otv.View(graph)

# %%
# We can also use the profile log-likehood function rather than log-likehood function  to estimate the parameters of the GEV.
result_PLL_max = factory.buildMethodOfProfileLikelihoodMaximizationEstimator(sample)

# %%
# The following graph allows one to get the profile log-likelihood plot.
# It also indicates the optimal value of :math:`\xi`, the maximum profile log-likelihood and
# the confidence interval for :math:`\xi` of order 0.95 (which is the default value).
order = 0.95
result_PLL_max.setConfidenceLevel(order)
print(result_PLL_max.getParameterConfidenceInterval())

# %%
# We can get the numerical values of the confidence interval: it appears to be a bit smaller
# with the interval obtained from the profile log-likelihood function than with the log-likelihood function.
print('Confidence interval for xi = ', result_PLL_max.getParameterConfidenceInterval())


# %%
# **Stationary GEV modeling from the largest :math:`r` annual sea-levels**
#
# We still assume that the dependence through time is negligible.  We estimate the parameters of the
# GEV distribution by maximizing the log-likelihood of the data. Now, we want to model more of the
# observed extremes than the annual maxima: the additional information contained in the largest
# :math:`10` observations can be used to improve the estimation of the GEV model.
#
# Now, we drop the year column to keep only the maxima values
sample_rmax = data[:, 1:]
print(sample_rmax[:5])

# %%
# We estimate the parameters from the largest :math:`r` annual sea-levels for :math:`r=1,5,10`. For each :math:`r` value, we get the estimated parameters.
factory = ot.GeneralizedExtremeValueFactory()
r_candidate = [1, 5, 10]
for r in r_candidate:
    estimate = factory.buildRMaxima(sample_rmax, r)
    desc = estimate.getParameterDescription()
    p = estimate.getParameter()
    pretty_p = ", ".join([f"{param}: {value:.3f}" for param, value in zip(desc, p)])
    print(f"r={r:2} {pretty_p}")

# %%
# If we want to prioritise the models with respect to the log-likelihood value and get the best model,
# we can use the following method. It also stores the log-likelihood value associated to each model.
best_r, llh = factory.buildBestRMaxima(sample_rmax, r_candidate)
print('Best model obtained for r = ', best_r)
for i in range(3):
    print(f"r={r_candidate[i]} likelihood={llh[i]}")

# %%
otv.View.ShowAll()

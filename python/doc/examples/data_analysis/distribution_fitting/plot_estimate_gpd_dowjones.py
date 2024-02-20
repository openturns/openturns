"""
Estimate a GPD on the Dow Jones Index data
==========================================
"""
# %%
# In this example, we illustrate various techniques of extreme value modeling applied
# to the 5-year series of daily Dow Jones Index closing prices.
# Readers should refer to [coles2001]_ example 1.8 to get more details.
#
# We illustrate techniques to:
#
# - estimate a stationary and a non stationary GPD,
# - estimate a return level,
#
# using:
#
# - the log-likelihood function,
# - the profile log-likelihood function.
#
import math as m
import openturns as ot
import openturns.experimental as otexp
import openturns.viewer as otv
from openturns.usecases import coles

# %%
# First, we load the Dow Jones dataset and plot it through time.
sample = coles.Coles().dowjones
print(sample[:10])
graph = ot.Graph(
    "Daily closing prices of the Dow Jones Index", "Day index", "Index", True, ""
)
# multiply by a factor to account for non-working days missing values so the period range matches
size = len(sample)
days = ot.Sample([[i] for i in range(size)])
curve = ot.Curve(days, sample)
curve.setColor("red")
graph.add(curve)
graph.setIntegerXTick(True)
view = otv.View(graph)

# %%
# Transform the raw data into a series reasonably stationary.
tsample = ot.Sample([[m.log(sample[i, 0]) - m.log(sample[i - 1, 0])] for i in range(1, size)])
curve = ot.Curve(days[:-1], tsample)
graph.setDrawable(curve, 0)
view = otv.View(graph)


# %%
# Rescale the data for convenience
sample = tsample * 100.0
size = len(sample)

# %%
# Draw the mean residual life plot
# The curve is initially linear and show significant curvature in [-1, 2] the relatively linear for :math:`u>2`
factory = ot.GeneralizedParetoFactory()
graph = factory.drawMeanResidualLife(tsample)
view = otv.View(graph)
u = 2.0

# %%
# **Stationary GPD modeling via the log-likelihood function**
#
# We first assume that the dependence through time is negligible, so we first model the data as
# independent observations over the observation period.
# We estimate the parameters of the GPD distribution by maximizing the log-likelihood of the data for a given threshold u.
result_LL = factory.buildMethodOfLikelihoodMaximizationEstimator(sample, u)

# %%
# We get the fitted GPD and its parameters of :math:`(\hat{\sigma}, \hat{\xi}, u)`.
fitted_GPD = result_LL.getDistribution()
desc = fitted_GPD.getParameterDescription()
param = fitted_GPD.getParameter()
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
# At last, we can validate the inference result thanks to the 4 usual diagnostic plots.
validation = otexp.GeneralizedParetoValidation(result_LL, sample)
graph = validation.drawDiagnosticPlot()
view = otv.View(graph)


# %%
# Find consecutive exceedances clusters and the associated peaks for a minimum gap r=4
r = 4
peaks, clusters = factory.getPeakOverThresholdWithClusters(sample, u, r)
nc = len(peaks)
nu = sum([1 if sample[i, 0] > u else 0 for i in range(size)])
print(f"nc={nc} nu={u} theta={nc/nu:.2f}")

# %%
otv.View.ShowAll()

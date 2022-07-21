"""
Estimate a multivariate ARMA process
====================================
"""
# %%
# The objective of the Use Case is to estimate a multivariate ARMA model
# from a stationary time series using the maximum likelihood estimator
# and a centered normal white noise.
# The data can be a unique time series or several time series collected
# in a process sample.
#
# We estimate :math:`(\underline{\beta}, \sigma^2)` thanks to the
# *ARMALikelihoodFactory* object and its method *build*, acting on a
# time series or on a sample of time series. It produces a result of
# type *ARMA*.
# Note that no evaluation of selection criteria such as *AIC* and *BIC*
# is done.
#
# The synthetic data is generated from the 2-d ARMA model:
#
# .. math::
#    \begin{aligned}
#    X_{0,t} - 0.5 X_{0,t-1} - 0.1 X_{1,t-1} = E_{0,t} - 0.4 E_{0,t-1} \\
#    X_{1,t} - 0.4 X_{0,t-1} - 0.5 X_{1,t-1} - 0.25 X_{0,t-2} = E_{1,t} - 0.4 E_{1,t-1}
#    \end{aligned}
#
# with E the white noise:
#
# .. math::
#     E \sim \mathcal{N} ([0,0], [0.1,0.2])
#

# %%
import openturns as ot
ot.Log.Show(ot.Log.NONE)

# %%
# Create a 2-d ARMA process
p = 2
q = 1
dim = 2

# Tmin , Tmax and N points for TimeGrid
dt = 1.0
size = 400
timeGrid = ot.RegularGrid(0.0, dt, size)

# white noise
cov = ot.CovarianceMatrix([[0.1, 0.0], [0.0, 0.2]])
whiteNoise = ot.WhiteNoise(ot.Normal([0.0] * dim, cov), timeGrid)

# AR/MA coefficients
ar = ot.ARMACoefficients(p, dim)
ar[0] = ot.SquareMatrix([[-0.5, -0.1], [-0.4, -0.5]])
ar[1] = ot.SquareMatrix([[0.0, 0.0], [-0.25, 0.0]])

ma = ot.ARMACoefficients(q, dim)
ma[0] = ot.SquareMatrix([[-0.4, 0.0], [0.0, -0.4]])

# ARMA model creation
arma = ot.ARMA(ar, ma, whiteNoise)
arma

# %%
# Create a realization
timeSeries = ot.TimeSeries(arma.getRealization())

# %%
# Estimate the process from the previous realization
factory = ot.ARMALikelihoodFactory(p, q, dim)
factory.setInitialConditions(ar, ma, cov)

arma_est = ot.ARMA(factory.build(timeSeries))
print('Estimated ARMA= ', arma_est)

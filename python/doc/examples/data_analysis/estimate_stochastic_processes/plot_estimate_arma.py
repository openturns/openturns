"""
Estimate a scalar ARMA process
==============================
"""
# %%
# The objective here is to estimate an ARMA model from a scalar stationary
# time series using the Whittle estimator and a centered normal white
# noise.
#
# The data can be a unique time series or several time series collected in
# a process sample.
#
# If the user specifies the order :math:`(p,q)`, OpenTURNS fits a model
# :math:`ARMA(p,q)` to the data by estimating the coefficients
# :math:`(a_1, \dots, a_p), (b_1, \dots, b_q)` and the variance :math:`\sigma` of the
# white noise.
#
# If the User specifies a range of orders :math:`(p,q) \in Ind_p \times Ind_q`,
# where :math:`Ind_p = [p_1, p_2]` and :math:`Ind_q = [q_1, q_2]`, We find the *best*
# model :math:`ARMA(p,q)` that fits to the data and estimates the corresponding
# coefficients.
#
# We proceed as follows:
#
# -   the object *WhittleFactory* is created with either a specified order
#     :math:`(p,q)` or a range :math:`Ind_p \times Ind_q`. By default, the Welch
#     estimator (object *Welch*) is used with its default parameters.
# -   for each order :math:`(p,q)`, the estimation of the parameters is done by
#     maximizing the reduced equation of the Whittle likelihood function
#     (\[lik2\]), thanks to the method *build* of the object
#     *WhittleFactory*. This method applies to a time series or a process
#     sample. If the user wants to get the quantified criteria
#     :math:`AIC_c, AIC` and *BIC* of the model :math:`ARMA(p,q)`, he has to specify
#     it by giving a *Point* of size 0 (*Point()*) as input parameter of
#     the method *build*.
# -   the output of the estimation is, in all the cases, one unique ARMA:
#     the ARMA with the specified order or the optimal one with respect to
#     the :math:`AIC_c` criterion.
# -   in the case of a range :math:`Ind_p \times Ind_q`, the user can get all
#     the estimated models thanks to the method *getHistory* of the object
#     *WhittleFactory*. If the *build* has been parameterized by a *Point*
#     of size 0, the user also has access to all the quantified criteria.
#
# The synthetic data is generated using the following 1-d ARMA process:
#
# .. math::
#     X_{0,t} + 0.4 X_{0,t-1} + 0.3 X_{0,t-2} + 0.2 X_{0,t-3} + 0.1 X_{0,t-4} = E_{0,t} + 0.4 E_{0,t-1} + 0.3 E_{0,t-2}
#
# with the noise :math:`E` defined as:
#
# .. math::
#     E \sim Triangular(-1, 0, 1)
#

# %%
import openturns as ot
import matplotlib.pyplot as plt
ot.RandomGenerator.SetSeed(0)
ot.Log.Show(ot.Log.NONE)

# %%
# Create an arma process

tMin = 0.0
n = 1000
timeStep = 0.1
myTimeGrid = ot.RegularGrid(tMin, timeStep, n)

myWhiteNoise = ot.WhiteNoise(ot.Triangular(-1.0, 0.0, 1.0), myTimeGrid)
myARCoef = ot.ARMACoefficients([0.4, 0.3, 0.2, 0.1])
myMACoef = ot.ARMACoefficients([0.4, 0.3])
arma = ot.ARMA(myARCoef, myMACoef, myWhiteNoise)

tseries = ot.TimeSeries(arma.getRealization())

# Create a sample of N time series from the process
N = 100
sample = arma.getSample(N)

# %%
# CASE 1 : we specify a (p,q) order

# Specify the order (p,q)
p = 4
q = 2

# Create the estimator
factory = ot.WhittleFactory(p, q)
print("Default spectral model factory = ", factory.getSpectralModelFactory())

# To set the spectral model factory
# For example, set WelchFactory as SpectralModelFactory
# with the Hann filtering window
# The Welch estimator splits the time series in four blocs without overlap
myFilteringWindow = ot.Hann()
mySpectralFactory = ot.WelchFactory(myFilteringWindow, 4, 0)
factory.setSpectralModelFactory(mySpectralFactory)
print("New spectral model factory = ", factory.getSpectralModelFactory())

# Estimate the ARMA model from a time series
# To get the quantified AICc, AIC and BIC criteria
arma42, criterion = factory.buildWithCriteria(tseries)
AICc, AIC, BIC = criterion[0:3]
print('AICc=', AICc, 'AIC=', AIC, 'BIC=', BIC)
arma42

# %%
# CASE 2 : we specify a range of (p,q) orders

# Range for p
pIndices = [1, 2, 4]
# Range for q = [4,5,6]
qIndices = [4, 5, 6]

# Build a Whittle factory with default SpectralModelFactory (WelchFactory)
# this time using ranges of order p and q
factory_range = ot.WhittleFactory(pIndices, qIndices)

# Estimate the arma model from a process sample
arma_range, criterion = factory_range.buildWithCriteria(sample)
AICc, AIC, BIC = criterion[0:3]
print('AICc=', AICc, 'AIC=', AIC, 'BIC=', BIC)
arma_range

# %%
# Results exploitation

# Get the white noise of the (best) estimated arma
arma_range.getWhiteNoise()

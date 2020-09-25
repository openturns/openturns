"""
Calibration of the logistic model
=================================
"""
# %%
# Introduction
# ------------
#
# The logistic growth model is the differential equation:
#
# .. math::
#    \frac{dy(t)}{dt} = ay(t) - by(t)^2
# 
#
# for any :math:`t\in[t_0, t_{final}]`, with the initial condition:
#
# .. math::
#    y(t_0) = y_0
# 
#
# where
#
# - :math:`a > 0` and :math:`b > 0` are two real parameters, 
# - :math:`y(t)` is the size of the population at time :math:`t`, 
# - :math:`t_0` is the initial time,
# - :math:`y_0` is the initial population at time :math:`t=t_0`, 
# - :math:`t_{final}` is the final time.
#
# The :math:`a` parameter sets the growth rate of the population. The :math:`b` parameter acts as a competition parameter which limits the size of the population by increasing the competition between its members. 
#
# In [1], the author uses this model to simulate the growth of the U.S. population. To do this, the author uses the U.S. census data from 1790 to 1910. For this time interval, R. Pearl and L. Reed [2] computed the following values of the parameters:
#
# .. math::
#    a = 0.03134, \qquad b = 1.5887 \times 10^{-10}.
# 
#
# Our goal is to use the logistic growth model in order to simulate the solution for a larger time interval, from 1790 to 2000:
#
# .. math::
#    t_0 = 1790, \qquad t_{final} = 2000.
# 
#
# Then we can compare the predictions of this model with the real evolution of the U.S. population.
#
# We can prove that, if :math:`y_0 > 0`, then the limit population is:
#
# .. math::
#    y_{limit} =\frac{a}{b}.
# 
#
# In 1790, the U.S. population was 3.9 Millions inhabitants:
#
# .. math::
#    y_0 = 3.9 \times 10^6.
# 
#
# We can prove that the exact solution of the ordinary differential equation is:
#
# .. math::
#    y(t)=\frac{ay_0}{by_0+(a-by_0 ) \exp(-a(t-t_0)) }
# 
#
# for any :math:`t\in[t_0, t_{final}]`.
#
# We want to see the solution of the ordinary differential equation when uncertainties are taken into account in the parameters:
#
# - the initial U.S. population :math:`y_0`,
# - the parameters :math:`a` and :math:`b`.
#
# Indeed, Pearl and Reed [2] estimated the parameters :math:`a` and :math:`b` using the U.S. census data from 1790 to 1910 while we have the data up to 2000. Moreover, the method used by Pearl and Reed to estimate the parameters could be improved; they only used 3 dates to estimate the parameters instead of using least squares, for example. Finally, Pearl and Reed did not provide confidence intervals for the parameters :math:`a` and :math:`b`. 

# %%
#
# Normalizing the data
# --------------------
#
# The order of magnitude of :math:`a` and :math:`b` are very different. In order to mitigate this, we consider the parameter :math:`c` as the logarithm of :math:`b`:
#
# .. math::
#    c = \log(b).
# 
#
# This leads to the value:
#
# .. math::
#    c = \log\left(1.5887 \times 10^{-10}\right) = -22.58.
# 
#
# The order of magnitude of the population is :math:`10^6`. This is why we consider the normalized population in millions:
#
# .. math::
#    z(t) = \frac{y(t)}{10^6}
# 
#
# for any :math:`t\in[t_0, t_{final}]`.
#
# Let :math:`z_0` be the initial population:
#
# .. math::
#    z_0 = z(t_0).
# 

# %%
# Uncertainties
# -------------
#
# Uncertainty can be accounted for by turning :math:`z_0`, :math:`a` and :math:`c` into independent random variables :math:`Z_0`, :math:`A` and :math:`C` with Gaussian distributions. From this point onward, :math:`z_0`, :math:`a` and :math:`b` respectively denote :math:`\mathbb{E}[Z_0]`, :math:`\mathbb{E}[A]` and :math:`\mathbb{E}[C]`.
#
# ===========   ===============================================================
# Variable      Distribution
# ===========   ===============================================================
# :math:`Z_0`   gaussian, mean :math:`z_0`, coefficient of variation 10% 
# :math:`A`     gaussian, mean :math:`a`, coefficient of variation 30% 
# :math:`C`     gaussian, mean :math:`c`, coefficient of variation 30% 
# ===========   ===============================================================
#
# No particular probabilistic method was used to set these distributions. An improvement would be to use calibration methods to get a better quantification of these distributions. An improvement would be to use calibration methods to get a better quantification of these distributions. 

# %%
# Notes
# -----
#
# * This example is based on [1], chapter "First order differential equations", page 28. 
# The data used in [1] are from [3]. 
# The logistic growth model was first suggested by Pierre François Verhulst near 1840. 
# The data are from [1] for the time interval from 1790 to 1950, then from [2] for the time interval from 1960 to 2000.
# * Calibrating this model may require to take into account for the time dependency of the measures.

# %%
# References
# ----------
#
# [1] Martin Braun. Differential equations and their applications, Fourth Edition. Texts in applied
# mathematics. Springer, 1993.
#
# [2] Cleve Moler. Numerical Computing with Matlab. Society for Industrial Applied Mathematics,
# 2004.
#
# [3] Raymond Pearl and Lowell Reed. On the rate of growth of the population of the united states
# since 1790 and its mathematical representation. Proceedings of the National Academy of Sciences,
# 1920.

# %%
# Analysis of the data
# --------------------

# %%
import openturns as ot
import numpy as np
import openturns.viewer as viewer
from matplotlib import pylab as plt

# %%
# The data is based on 22 dates from 1790 to 2000.

# %%
observedSample=ot.Sample([\
[1790,3.9], \
[1800,5.3], \
[1810,7.2], \
[1820,9.6], \
[1830,13], \
[1840,17], \
[1850,23], \
[1860,31], \
[1870,39], \
[1880,50], \
[1890,62], \
[1900,76], \
[1910,92], \
[1920,106], \
[1930,123], \
[1940,132], \
[1950,151], \
[1960,179], \
[1970,203], \
[1980,221], \
[1990,250], \
[2000,281]])

# %%
nbobs = observedSample.getSize()
nbobs

# %%
timeObservations = observedSample[:,0]
timeObservations[0:5]

# %%
populationObservations = observedSample[:,1]
populationObservations[0:5]

# %%
graph = ot.Graph('', 'Time (years)', 'Population (Millions)', True, 'topleft')
cloud = ot.Cloud(timeObservations, populationObservations)
cloud.setLegend("Observations")
graph.add(cloud)
view = viewer.View(graph)

# %%
# We consider the times and populations as dimension 22 vectors. The `logisticModel` function takes a dimension 24 vector as input and returns a dimension 22 vector. The first 22 components of the input vector contains the dates and the remaining 2 components are $a$ and $b$. 

# %%
nbdates = 22
def logisticModel(X):
    t = [X[i] for i in range(nbdates)]
    a = X[22]
    c = X[23]
    t0 = 1790.
    y0 = 3.9e6
    b = np.exp(c)
    y = ot.Point(nbdates)
    for i in range(nbdates):
        y[i] = a*y0/(b*y0+(a-b*y0)*np.exp(-a*(t[i]-t0)))
    z = y/1.e6 # Convert into millions
    return z


# %%
logisticModelPy = ot.PythonFunction(24, nbdates, logisticModel)

# %%
# The reference values of the parameters. 
#
# Because :math:`b` is so small with respect to :math:`a`, we use the logarithm. 

# %%
np.log(1.5587e-10)

# %%
a=0.03134
c=-22.58
thetaPrior = [a,c]

# %%
logisticParametric = ot.ParametricFunction(logisticModelPy,[22,23],thetaPrior)

# %%
# Check that we can evaluate the parametric function.

# %%
populationPredicted = logisticParametric(timeObservations.asPoint())
populationPredicted

# %%
graph = ot.Graph('', 'Time (years)', 'Population (Millions)', True, 'topleft')
# Observations
cloud = ot.Cloud(timeObservations, populationObservations)
cloud.setLegend("Observations")
cloud.setColor("blue")
graph.add(cloud)
# Predictions
cloud = ot.Cloud(timeObservations.asPoint(), populationPredicted)
cloud.setLegend("Predictions")
cloud.setColor("green")
graph.add(cloud)
view = viewer.View(graph)

# %%
# We see that the fit is not good: the observations continue to grow after 1950, while the growth of the prediction seem to fade.

# %% [markdown]
# Calibration with linear least squares
# -------------------------------------

# %%
timeObservationsVector = ot.Sample([[timeObservations[i,0] for i in range(nbobs)]])
timeObservationsVector[0:10]

# %%
populationObservationsVector = ot.Sample([[populationObservations[i, 0] for i in range(nbobs)]])
populationObservationsVector[0:10]

# %%
# The reference values of the parameters. 

# %%
a=0.03134
c=-22.58
thetaPrior = ot.Point([a,c])


# %%
logisticParametric = ot.ParametricFunction(logisticModelPy,[22,23],thetaPrior)

# %%
# Check that we can evaluate the parametric function.

# %%
populationPredicted = logisticParametric(timeObservationsVector)
populationPredicted[0:10]

# %%
#  Calibration
# ------------

# %%
algo = ot.LinearLeastSquaresCalibration(logisticParametric, timeObservationsVector, populationObservationsVector, thetaPrior)

# %%
algo.run()

# %%
calibrationResult = algo.getResult()

# %%
thetaMAP = calibrationResult.getParameterMAP()
thetaMAP

# %%
thetaPosterior = calibrationResult.getParameterPosterior()
thetaPosterior.computeBilateralConfidenceIntervalWithMarginalProbability(0.95)[0]

# %%
# transpose samples to interpret several observations instead of several input/outputs as it is a field model
if calibrationResult.getInputObservations().getSize() == 1:
    calibrationResult.setInputObservations([timeObservations[i] for i in range(nbdates)])
    calibrationResult.setOutputObservations([populationObservations[i] for i in range(nbdates)])
    outputAtPrior = [[calibrationResult.getOutputAtPriorMean()[0, i]] for i in range(nbdates)]
    outputAtPosterior = [[calibrationResult.getOutputAtPosteriorMean()[0, i]] for i in range(nbdates)]
    calibrationResult.setOutputAtPriorAndPosteriorMean(outputAtPrior, outputAtPosterior)

# %%
graph = calibrationResult.drawObservationsVsInputs()
view = viewer.View(graph)

# %%
graph = calibrationResult.drawObservationsVsInputs()
view = viewer.View(graph)

# %%
graph = calibrationResult.drawObservationsVsPredictions()
view = viewer.View(graph)

# %%
graph = calibrationResult.drawResiduals()
view = viewer.View(graph)

# %%
graph = calibrationResult.drawParameterDistributions()
view = viewer.View(graph)

if not viewer._noshow:
    plt.show()

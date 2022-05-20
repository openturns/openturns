#!/usr/bin/env python

"""
Calibration without observed inputs
===================================
"""
# %%
# The goal of this example is to present the calibration of a parametric model which 
# does not have observed inputs.
# We are going to see how to use the :class:`~openturns.Sample` class 
# and create an empty sample. 
# Indeed, this is mandatory in order to fit within the calibration 
# framework that is used in the library. 
# In this example there are, however, several outputs to be calibrated.

import openturns as ot
from matplotlib import pylab as plt
import openturns.viewer as viewer

# %%
# The vector of parameters is 
# :math:`\boldsymbol{\theta} = (a, b, c)^T \in \mathbb{R}^3`. 
# This model is linear in :math:`\boldsymbol{\theta}` and identifiable.
# It is derived from the equation:
# 
# .. math::
#     y(x) = a + b x + c x^2
# 
# at :math:`x=-1.0, -0.6, -0.2, 0.2, 0.6, 1.0`. 
# In the model, however, the abscissas are fixed and constant. 
# Therefore, the parametric model has 3 parameters, no input and 6 outputs 
# :math:`y_1, ..., y_6`.
# This produces a set of 5 observations for each output, leading to a total 
# of 5 (observations per output) * 6 (outputs) = 30 observations.
g = ot.SymbolicFunction(
    ["a", "b", "c"],
    [
        "a +  -1.0  * b +  1.0  * c",
        "a +  -0.6  * b +  0.36  * c",
        "a +  -0.2  * b +  0.04  * c",
        "a +  0.2  * b +  0.04  * c",
        "a +  0.6  * b +  0.36  * c",
        "a +  1.0  * b +  1.0  * c",
    ],
)
outputDimension = g.getOutputDimension()
print(outputDimension)

# %%
# We set the true value of the parameters.
trueParameter = ot.Point([12.0, 7.0, -8.0])
print(trueParameter)

# %%
# In order to generate the observed outputs, we create a distribution 
# in dimension 3, with Dirac (i.e. constant) marginals.
inputRandomVector = ot.ComposedDistribution([ot.Dirac(theta) for theta in trueParameter])

# %%
# The candidate value is chosen to be different from the true parameter value. 
candidate = ot.Point([8.0, 9.0, -6.0])
calibratedIndices = [0, 1, 2]
model = ot.ParametricFunction(g, calibratedIndices, candidate)

# %%
# We consider a multivariate gaussian noise with zero mean, standard deviation 
# equal to 0.05 and independent copula.
# The independent copula implies that the errors of the 6 outputs are independent.
outputObservationNoiseSigma = 1.0
meanNoise = ot.Point(outputDimension)
covarianceNoise = [outputObservationNoiseSigma] * outputDimension
R = ot.IdentityMatrix(outputDimension)
observationOutputNoise = ot.Normal(meanNoise, covarianceNoise, R)
print(observationOutputNoise)

# %%
# Finally, we generate the outputs by evaluating the exact outputs of the 
# function and adding the noise.
# We use a sample with size equal to 5.
size = 5
# Generate exact outputs
inputSample = inputRandomVector.getSample(size)
outputStress = g(inputSample)
# Add noise
sampleNoise = observationOutputNoise.getSample(size)
outputObservations = outputStress + sampleNoise

# %%
# Now is the important part of this script : there are no input observations. 
# This is why we create a sample with size equal to 5 and dimension equal to 0. 
# Even if the calibration model has no input observations, observed inputs 
# are required by the current programming interface and this is why we have to 
# create this object. 
inputObservations = ot.Sample(size, 0)  # Trick

# %%
# We are now ready to perform the calibration.
algo = ot.LinearLeastSquaresCalibration(
    model, inputObservations, outputObservations, candidate, "SVD"
)
algo.run()
calibrationResult = algo.getResult()
parameterMAP = calibrationResult.getParameterMAP()
print(parameterMAP)


# %%
# We observe that the estimated parameter is relatively close to 
# the true parameter value.
print(parameterMAP - trueParameter)


# %%
# Graphical validation
# --------------------
#
# We now validate the calculation by drawing the exact function and compare 
# it to the function with estimated parameters.
fullModel = ot.SymbolicFunction(["a", "b", "c", "x"], ["a + b * x + c * x^2"])
parameterIndices = [0, 1, 2]
trueFunction = ot.ParametricFunction(fullModel, parameterIndices, trueParameter)
print(trueFunction)
beforeCalibrationFunction = ot.ParametricFunction(fullModel, parameterIndices, candidate)
print(beforeCalibrationFunction)
calibratedFunction = ot.ParametricFunction(fullModel, parameterIndices, parameterMAP)
print(calibratedFunction)

# %%
# In order to validate the calibration, we compare the parametric function 
# with true parameters at given abscissas with the parametric function 
# with calibrated parameters. 
abscissas = [-1.0, -0.6, -0.2, 0.2, 0.6, 1.0]
xmin = min(abscissas)
xmax = max(abscissas)

npoints = 50
graph = ot.Graph("Calibration without observations", "x", "y", True, "bottomright")
curve = trueFunction.draw(xmin, xmax, npoints).getDrawable(0)
curve.setLineStyle("dashed")
curve.setLegend("True model")
curve.setColor("darkorange1")
graph.add(curve)
# Before calibration
curve = beforeCalibrationFunction.draw(xmin, xmax, npoints)
curve.setLegends(["Model before calibration"])
curve.setColors(["red"])
graph.add(curve)
# After calibration
curve = calibratedFunction.draw(xmin, xmax, npoints)
curve.setLegends(["Model after calibration"])
curve.setColors(["green"])
graph.add(curve)
# Observations
for i in range(outputDimension):
    cloud = ot.Cloud(ot.Sample([[abscissas[i]]] * size), outputObservations[:, i])
    cloud.setColor("blue")
    if i == 0:
        cloud.setLegend("Observations")
    graph.add(cloud)
viewer = viewer.View(graph)

plt.show()


# %%
# We notice that the calibration produces a good fit to the data. 
# Still, we notice a small discrepancy between the true mode and the model 
# after calibration, but this discrepancy is very small.
# Since the model is linear with respect to the parameters :math:`a`, :math:`b`, :math:`c`, 
# therefore the LinearLeastSquares method performs well. 
# If the modal was non linear w.r.t. :math:`a`, :math:`b`, :math:`c`, therefore the linear least 
# squares method would not work that well and the parameters would be estimated 
# with less accuracy.

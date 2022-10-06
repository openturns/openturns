#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# Default dimension parameter to evaluate the model
defaultDimension = 1

# Amplitude values
amplitude = ot.Point(defaultDimension, 1.0)
# Scale values
scale = ot.Point(defaultDimension, 1.0)

# Spectral density
myModel = ot.CauchyModel(scale, amplitude)

points = 8
tMin = 0.0
tStep = 1.0 / (points - 1)

# RegularGrid --> Build list of frequencies using the RegularGrid
myTimeGrid = ot.RegularGrid(tMin, tStep, points)

mySpectralProcess0 = ot.SpectralGaussianProcess(myModel, myTimeGrid)

print("mySpectralProcess0 = ", mySpectralProcess0)
print("Realization = ", mySpectralProcess0.getRealization())

# Constructor using maximalFrequency value and size of discretization
maximalFrequency = 10.0
mySpectralProcess1 = ot.SpectralGaussianProcess(
    myModel, maximalFrequency, points)
tg = ot.RegularGrid(mySpectralProcess1.getTimeGrid())

print("mySpectralProcess1 = ", mySpectralProcess1)
print("Realization = ", mySpectralProcess1.getRealization())

# 3D model
highDimension = 3
amplitude = ot.Point(highDimension, 1.0)

# Second order model with parameters
mySpecModel = ot.CauchyModel(scale, amplitude)
print("mySpecModel = ", mySpecModel)

mySpectralProcess2 = ot.SpectralGaussianProcess(mySpecModel, myTimeGrid)
print("mySpectralProcess2 = ", mySpectralProcess2)
print("Realization = ", mySpectralProcess2.getRealization())
mySpectralProcess3 = ot.SpectralGaussianProcess(
    mySpecModel, maximalFrequency, points)
print("mySpectralProcess3 = ", mySpectralProcess3)
print("Realization = ", mySpectralProcess3.getRealization())


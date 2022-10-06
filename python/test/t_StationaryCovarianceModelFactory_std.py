#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# Dimension of the input model
# Size of the TimeGrid
# dimension parameter
dimension = 1

# Amplitude values
amplitude = ot.Point(dimension, 1.00)

# Scale values
scale = ot.Point(dimension, 1.0)

size = 128
timeGrid = ot.RegularGrid(0., 0.1, size)

# Cauchy model
model = ot.CauchyModel(scale, amplitude)
covModel = ot.AbsoluteExponential(scale, amplitude)
myProcess = ot.SpectralGaussianProcess(model, timeGrid)

# Create a sample of size N = 1000
sample = myProcess.getSample(1000)

# StationaryCovarianceModelFactory using default parameter - Factory
# initiate
myFactory = ot.StationaryCovarianceModelFactory()

# Build a UserDefinedCovarianceModel using the Wellch method
myCovarianceModel = myFactory.buildAsUserDefinedStationaryCovarianceModel(
    sample)
tg = myCovarianceModel.getTimeGrid()

# Get the time grid of the model
for i in range(tg.getN()):
    t = tg.getValue(i)
    estimatedValue = myCovarianceModel(t)[0, 0]
    modelValue = covModel(t)[0, 0]
    print("Covariance C( %.6g" % t, ") : ", " evaluation =  %.6g" %
          estimatedValue, " model =  %.6g" % modelValue)

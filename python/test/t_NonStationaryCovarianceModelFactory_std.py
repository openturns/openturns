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

size = 10
timeGrid = ot.RegularGrid(0.0, 0.1, size)

# Absolute model
model = ot.AbsoluteExponential(scale, amplitude)
myProcess = ot.GaussianProcess(model, timeGrid)

# Create a Process sample of size N
N = 10000
sample = myProcess.getSample(N)

# NonStationaryCovarianceModelFactory using default parameter - Factory
# initiate
myFactory = ot.NonStationaryCovarianceModelFactory()

# Build a UserDefinedCovarianceModel using the Welch method
myCovarianceModel = myFactory.buildAsUserDefinedCovarianceModel(sample)

for i in range(size):
    t = timeGrid.getValue(i)
    for j in range(size):
        s = timeGrid.getValue(j)
        estimatedValue = myCovarianceModel(t, s)[0, 0]
        modelValue = model(t, s)[0, 0]
        print(
            "Covariance C( %.6g" % t,
            ",  %.6g" % s,
            ") : ",
            " evaluation =  %.6g" % estimatedValue,
            " model =  %.6g" % modelValue,
        )

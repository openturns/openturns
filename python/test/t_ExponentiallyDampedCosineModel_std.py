#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# Default dimension parameter to evaluate the model
defaultDimension = 1
inputDimension = 1

# Amplitude values
amplitude = [1.0]

# Scale values
scale = [1.0]

# Frequency values
for frequency in [0.1, 0.2]:
    # Default constructor
    myDefaultModel = ot.ExponentiallyDampedCosineModel()
    print("myDefaultModel = ", myDefaultModel)

    # Second order model with parameters
    myModel = ot.ExponentiallyDampedCosineModel(
        scale, amplitude, frequency)
    print("myModel = ", myModel)

    timeValueOne = 1.
    print("covariance matrix at t = ", timeValueOne,
          " : ", myModel(timeValueOne))
    print("covariance matrix at t = ", -1.0 * timeValueOne,
          " : ", myModel(-1.0 * timeValueOne))

    # Evaluation at time higher to check the decrease of the
    # exponentiallyDampedCosine values
    timeValueHigh = 15.
    print("covariance matrix at t = ", timeValueHigh,
          " : ", myModel(timeValueHigh).__str__())

    timeGrid = ot.RegularGrid(0.0, 1.0 / 3.0, 4)
    print("discretized covariance over the time grid=",
          timeGrid, "is=", myModel.discretize(timeGrid))

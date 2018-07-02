#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:

    # Default dimension parameter to evaluate the model
    defaultDimension = 1
    inputDimension = 1

    # Amplitude values
    amplitude = [1.0]

    # Scale values
    scale = [1.0]

    # Frequency values
    frequency = 0.1

    # Default constructor
    myDefautModel = ExponentiallyDampedCosineModel()
    print("myDefautModel = ", myDefautModel)

    # Second order model with parameters
    myModel = ExponentiallyDampedCosineModel(
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

    timeGrid = RegularGrid(0.0, 1.0 / 3.0, 4)
    print("discretized covariance over the time grid=",
          timeGrid, "is=", myModel.discretize(timeGrid))

except:
    import sys
    print("t_ExponentiallyDampedCosineModel_std.py",
          sys.exc_info()[0], sys.exc_info()[1])

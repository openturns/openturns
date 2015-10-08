#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:

    # Default dimension parameter to evaluate the model
    defaultDimension = 1

    # Default spatial dimension
    spatialDimension = 1

    # Amplitude values
    amplitude = NumericalPoint(defaultDimension, 1.0)

    # Scale values
    scale = NumericalPoint(spatialDimension, 1.0)

    # Default constructor
    myDefautModel = ExponentialCauchy()
    print("myDefautModel = ", myDefautModel)

    # Second order model with parameters
    myModel = ExponentialCauchy(amplitude, scale)
    print("myModel = ", myModel)

    timeValueOne = 1.
    print("covariance matrix at t = ", timeValueOne,
          " : ", myModel.computeCovariance(timeValueOne))
    print("covariance matrix at t = ", -1.0 * timeValueOne,
          " : ", myModel.computeCovariance(-1.0 * timeValueOne))

    # Evaluation at time higher to check the decrease of the exponential values
    timeValueHigh = 15.
    print("covariance matrix at t = ", timeValueHigh, " : ",
          myModel.computeCovariance(timeValueHigh).__str__())

    timeGrid = RegularGrid(0.0, 1.0 / 3.0, 4)
    print("discretized covariance over the time grid=",
          timeGrid, "is=", myModel.discretize(timeGrid))

except:
    import sys
    print("t_ExponentialCauchy_std.py", sys.exc_info()[0], sys.exc_info()[1])

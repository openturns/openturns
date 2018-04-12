#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:

    # Dimension of the input model
    # Size of the TimeGrid
    # dimension parameter
    dimension = 1

    # Amplitude values
    amplitude = Point(dimension, 1.00)

    # Scale values
    scale = Point(dimension, 1.0)

    size = 10
    timeGrid = RegularGrid(0.0, 0.1, size)

    # Absolute model
    model = AbsoluteExponential(scale, amplitude)
    myProcess = GaussianProcess(model, timeGrid)

    # Create a Process sample of size N
    N = 10000
    sample = myProcess.getSample(N)

    # NonStationaryCovarianceModelFactory using default parameter - Factory
    # initiate
    myFactory = NonStationaryCovarianceModelFactory()

    # Build a UserDefinedCovarianceModel using the Welch method
    myCovarianceModel = myFactory.buildAsUserDefinedCovarianceModel(sample)

    for i in range(size):
        t = timeGrid.getValue(i)
        for j in range(size):
            s = timeGrid.getValue(j)
            estimatedValue = myCovarianceModel(t, s)[0, 0]
            modelValue = model(t, s)[0, 0]
            print("Covariance C( %.6g" % t, ",  %.6g" % s, ") : ", " evaluation =  %.6g" %
                  estimatedValue, " model =  %.6g" % modelValue)

except:
    import sys
    print("t_NonStationaryCovarianceModelFactory_std.py",
          sys.exc_info()[0], sys.exc_info()[1])

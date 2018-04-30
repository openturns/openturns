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

    size = 128
    timeGrid = RegularGrid(0., 0.1, size)

    # Cauchy model
    model = CauchyModel(scale, amplitude)
    covModel = AbsoluteExponential(scale, amplitude)
    myProcess = SpectralGaussianProcess(model, timeGrid)

    # Create a sample of size N = 1000
    sample = myProcess.getSample(1000)

    # StationaryCovarianceModelFactory using default parameter - Factory
    # initiate
    myFactory = StationaryCovarianceModelFactory()

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

except:
    import sys
    print("t_StationaryCovarianceModelFactory_std.py",
          sys.exc_info()[0], sys.exc_info()[1])

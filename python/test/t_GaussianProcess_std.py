#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    # Default dimension parameter to evaluate the model
    defaultDimension = 1
    spatialDimension = 1

    # Amplitude values
    amplitude = Point(defaultDimension, 1.0)
    # Scale values
    scale = Point(defaultDimension, 1.0)

    tmin = 0.0
    step = 0.1
    n = 11

    myTimeGrid = RegularGrid(tmin, step, n)
    size = 25

    # Second order model with parameters
    myCovModel = ExponentialModel(scale, amplitude)
    print("myCovModel = ", myCovModel)

    myProcess1 = GaussianProcess(myCovModel, myTimeGrid)
    print("myProcess1 = ", myProcess1)
    print("is stationary? ", myProcess1.isStationary())
    myProcess1.setSamplingMethod(0)
    print("mean over ", size, " realizations = ",
          myProcess1.getSample(size).computeMean())
    myProcess1.setSamplingMethod(2)
    print("mean over ", size, " realizations = ",
          myProcess1.getSample(size).computeMean())

    # With constant trend
    trend = TrendTransform(SymbolicFunction("t", "4.0"), myTimeGrid)
    myProcess2 = GaussianProcess(trend, myCovModel, myTimeGrid)
    myProcess2.setSamplingMethod(2)
    print("myProcess2 = ", myProcess2)
    print("is stationary? ", myProcess2.isStationary())
    print("mean over ", size, " realizations= ",
          myProcess2.getSample(size).computeMean())

    # With varying trend
    trend3 = TrendTransform(SymbolicFunction("t", "sin(t)"), myTimeGrid)
    myProcess3 = GaussianProcess(trend3, myCovModel, myTimeGrid)
    print("myProcess3 = ", myProcess3)
    print("is stationary? ", myProcess3.isStationary())
    myProcess3.setSamplingMethod(0)
    print("mean over ", size, " realizations = ",
          myProcess3.getSample(size).computeMean())
    myProcess3.setSamplingMethod(2)
    print("mean over ", size, " realizations = ",
          myProcess3.getSample(size).computeMean())

except:
    import sys
    print("t_GaussianProcess_std.py",
          sys.exc_info()[0], sys.exc_info()[1])

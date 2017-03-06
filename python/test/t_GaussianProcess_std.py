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
    amplitude = NumericalPoint(defaultDimension, 1.0)
    # Scale values
    scale = NumericalPoint(defaultDimension, 1.0)

    # Second order model with parameters
    myModel = ExponentialCauchy(scale, amplitude)
    print("myModel = ", myModel)

    # checking the copy-cast*/
    mySecondOrderModel = SecondOrderModel(myModel)
    print("mySecondOrderModel = ", mySecondOrderModel)

    tmin = 0.0
    step = 0.1
    n = 11

    myTimeGrid = RegularGrid(tmin, step, n)
    size = 25

    myProcess = GaussianProcess(myModel, myTimeGrid)
    print("myProcess = ", myProcess)
    print("is stationary? ", myProcess.isStationary())
    myProcess.setSamplingMethod(0)
    print("mean over ", size, " realizations = ",
          myProcess.getSample(size).computeMean())
    myProcess.setSamplingMethod(2)
    print("mean over ", size, " realizations = ",
          myProcess.getSample(size).computeMean())

    # Second order model with parameters
    myCovModel = ExponentialModel(scale, amplitude)
    print("myCovModel = ", myCovModel)

    myProcess1 = GaussianProcess(myCovModel, myTimeGrid)
    print("myProcess1 = ", myProcess1)
    print("is stationary? ", myProcess1.isStationary())
    myProcess.setSamplingMethod(0)
    print("mean over ", size, " realizations = ",
          myProcess.getSample(size).computeMean())
    myProcess.setSamplingMethod(2)
    print("mean over ", size, " realizations = ",
          myProcess.getSample(size).computeMean())

    # With constant trend
    trend = TrendTransform(NumericalMathFunction("t", "4.0"))
    myProcess2 = GaussianProcess(trend, myCovModel, myTimeGrid)
    myProcess2.setSamplingMethod(2)
    print("myProcess2 = ", myProcess2)
    print("is stationary? ", myProcess2.isStationary())
    print("mean over ", size, " realizations= ",
          myProcess2.getSample(size).computeMean())

    # With varying trend
    trend3 = TrendTransform(NumericalMathFunction("t", "sin(t)"))
    myProcess3 = GaussianProcess(trend3, myCovModel, myTimeGrid)
    print("myProcess3 = ", myProcess3)
    print("is stationary? ", myProcess3.isStationary())
    myProcess.setSamplingMethod(0)
    print("mean over ", size, " realizations = ",
          myProcess.getSample(size).computeMean())
    myProcess.setSamplingMethod(2)
    print("mean over ", size, " realizations = ",
          myProcess.getSample(size).computeMean())

except:
    import sys
    print("t_GaussianProcess_std.py",
          sys.exc_info()[0], sys.exc_info()[1])

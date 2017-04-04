#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:

    # Realization issued from a SpectralProcess
    dimension = 1

    # Parameters of the distribution
    N = 101
    t0 = 0.
    dt = 0.1
    myTimeGrid = RegularGrid(t0, dt, N)

    # Create a Sample
    # parameters of gaussien impose a few risk to get negative values
    mySample = Normal(10, 3).getSample(N)

    # get a realization from distribution
    myRealization = TimeSeries(myTimeGrid, mySample)

    # Create the lambda parameter
    lambdaVector = Point(dimension)
    for index in range(dimension):
        lambdaVector[index] = (index + 2.) * 0.1

    myBoxCox = BoxCoxTransform(lambdaVector)

    print("myBoxCox=", myBoxCox)

    # Get the input and output dimension
    print("myBoxCox input dimension = ", myBoxCox.getInputDimension())
    print("myBoxCox output dimension = ", myBoxCox.getOutputDimension())

    # Evaluation of the BoxCoxTransform on the realization
    print("input time series =")
    print(myRealization)
    print("output time series = ")
    print(myBoxCox(myRealization))

    # Call the getInverse method
    myInverseBoxCox = myBoxCox.getInverse()
    print("myInverseBoxCox = ", myInverseBoxCox)

    # Get the number of calls
    print("number of call(s) : ", myBoxCox.getCallsNumber())

except:
    import sys
    print("t_BoxCoxTransform_std.py", sys.exc_info()[0], sys.exc_info()[1])

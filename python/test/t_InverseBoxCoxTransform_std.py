#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:

    # Realization issued from a SpectralProcess
    dimension = 1

    # Parameters of the distribution
    N = 51
    t0 = 0.
    dt = 0.1
    myTimeGrid = RegularGrid(t0, dt, N)

    # Create a Sample
    # parameters of gaussien impose a few risk to get negative values
    mySample = Uniform().getSample(N)

    # get a realization from distribution
    myRealization = TimeSeries(myTimeGrid, mySample)

    # Create the lambda parameter
    lambdaVector = Point(dimension)
    for index in range(dimension):
        lambdaVector[index] = (index + 2.) * 0.1

    myInverseBoxCox = InverseBoxCoxTransform(lambdaVector)

    print("myInverseBoxCox=", myInverseBoxCox)

    # Get the input and output dimension
    print("myInverseBoxCox input dimension = ",
          myInverseBoxCox.getInputDimension())
    print("myInverseBoxCox output dimension = ",
          myInverseBoxCox.getOutputDimension())

    # Evaluation of the InverseBoxCoxTransform on the realization
    print("input time series =")
    print(myRealization)
    print("output time series = ")
    print(myInverseBoxCox(myRealization))

    # Call the getInverse method
    myBoxCox = myInverseBoxCox.getInverse()
    print("myBoxCox = ", myBoxCox)

    # Get the number of calls
    print("number of call(s) : ", myInverseBoxCox.getCallsNumber())

except:
    import sys
    print("t_InverseBoxCoxTransform_std.py",
          sys.exc_info()[0], sys.exc_info()[1])

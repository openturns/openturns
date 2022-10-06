#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


# Realization issued from a SpectralProcess
dimension = 1

# Parameters of the distribution
N = 101
t0 = 0.
dt = 0.1
myTimeGrid = ot.RegularGrid(t0, dt, N)

# Create a Sample
# parameters of gaussien impose a few risk to get negative values
mySample = ot.Normal(10, 3).getSample(N)

# get a realization from distribution
myRealization = ot.TimeSeries(myTimeGrid, mySample)

# Create the lambda parameter
lambdaVector = ot.Point(dimension)
for index in range(dimension):
    lambdaVector[index] = (index + 2.) * 0.1

myBoxCox = ot.BoxCoxTransform(lambdaVector)

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

#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# Realization issued from a SpectralProcess
dimension = 1

# Parameters of the distribution
N = 51
t0 = 0.
dt = 0.1
myTimeGrid = ot.RegularGrid(t0, dt, N)

# Create a Sample
# parameters of gaussien impose a few risk to get negative values
mySample = ot.Uniform().getSample(N)

# get a realization from distribution
myRealization = ot.TimeSeries(myTimeGrid, mySample)

# Create the lambda parameter
lambdaVector = ot.Point(dimension)
for index in range(dimension):
    lambdaVector[index] = (index + 2.) * 0.1

myInverseBoxCox = ot.InverseBoxCoxTransform(lambdaVector)

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

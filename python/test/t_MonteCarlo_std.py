#! /usr/bin/env python

import sys
import openturns as ot

ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(0)


def progress(percent):
    sys.stderr.write("-- progress=" + str(percent) + "%\n")


def stop():
    sys.stderr.write("-- stop?\n")
    return False


# We create a numerical math function
myFunction = ot.Function(["E", "F", "L", "I"], ["d"], ["-F*L^3/(3*E*I)"])

dim = myFunction.getInputDimension()

# We create a normal distribution point of dimension 1
mean = [0.0] * dim
# E
mean[0] = 50.0
# F
mean[1] = 1.0
# L
mean[2] = 10.0
# I
mean[3] = 5.0
sigma = [1.0] * dim
R = ot.IdentityMatrix(dim)
myDistribution = ot.Normal(mean, sigma, R)

# We create a 'usual' RandomVector from the Distribution
vect = ot.RandomVector(myDistribution)

# We create a composite random vector
output = ot.RandomVector(myFunction, vect)

# We create an Event from this RandomVector
myEvent = ot.ThresholdEvent(output, ot.Less(), -3.0)

# We create a Monte Carlo algorithm
myAlgo = ot.MonteCarlo(myEvent)
myAlgo.setMaximumOuterSampling(250)
myAlgo.setBlockSize(4)
myAlgo.setMaximumCoefficientOfVariation(0.1)

print("MonteCarlo=", myAlgo)

# Perform the simulation
myAlgo.run()

# Stream out the result
print("MonteCarlo result=", myAlgo.getResult())

# Use the standard deviation as a stoping rule
myAlgo = ot.MonteCarlo(myEvent)
myAlgo.setMaximumOuterSampling(250)
myAlgo.setBlockSize(4)
myAlgo.setMaximumCoefficientOfVariation(0.0)
myAlgo.setMaximumStandardDeviation(0.1)
myAlgo.setProgressCallback(progress)
myAlgo.setStopCallback(stop)

print("MonteCarlo=", myAlgo)

# Perform the simulation
myAlgo.run()

# Stream out the result
print("MonteCarlo result=", myAlgo.getResult())

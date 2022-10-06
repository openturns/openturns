#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


# We create a numerical math function
myFunction = ot.SymbolicFunction(
    ["E", "F", "L", "I"], ["-F*L^3/(3*E*I)"])

dim = myFunction.getInputDimension()

# We create a normal distribution point of dimension 1
mean = ot.Point(dim, 0.0)
# E
mean[0] = 50.0
# F
mean[1] = 1.0
# L
mean[2] = 10.0
# I
mean[3] = 5.0
sigma = ot.Point(dim, 1.0)
R = ot.IdentityMatrix(dim)
myDistribution = ot.Normal(mean, sigma, R)

# We create a 'usual' RandomVector from the Distribution
vect = ot.RandomVector(myDistribution)

# We create a composite random vector
output = ot.CompositeRandomVector(myFunction, vect)

# We create an Event from this RandomVector
myEvent = ot.ThresholdEvent(output, ot.Less(), -3)

# We create a Monte Carlo algorithm
myAlgo = ot.LHS(myEvent)
myAlgo.setMaximumOuterSampling(250)
myAlgo.setBlockSize(4)
myAlgo.setMaximumCoefficientOfVariation(0.1)

print("LHS=", myAlgo)

# Perform the simulation
myAlgo.run()

# Stream out the result
print("LHS result=", myAlgo.getResult())


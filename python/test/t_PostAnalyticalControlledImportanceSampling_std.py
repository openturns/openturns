#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


# We create a numerical math function
myFunction = ot.SymbolicFunction(("E", "F", "L", "I"), ("-F*L^3/(3.*E*I)",))

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

# We create a FORM algorithm
# The first parameter is an OptimizationAlgorithm
# The second parameter is an event
# The third parameter is a starting point for the design point research
myCobyla = ot.Cobyla()
myCobyla.setMaximumCallsNumber(400)
myAlgo = ot.FORM(myCobyla, myEvent, mean)

# Perform the simulation
myAlgo.run()

# Create a PostAnalyticalControlledImportanceSampling algorithm based on
# the previous FORM result
formResult = myAlgo.getResult()
mySamplingAlgo = ot.PostAnalyticalControlledImportanceSampling(formResult)
print("FORM probability= %.11g" % formResult.getEventProbability())
mySamplingAlgo.setMaximumOuterSampling(250)
mySamplingAlgo.setBlockSize(4)
mySamplingAlgo.setMaximumCoefficientOfVariation(0.1)

print("PostAnalyticalControlledImportanceSampling=", mySamplingAlgo)

mySamplingAlgo.run()

# Stream out the result
print("PostAnalyticalControlledImportanceSampling result=", mySamplingAlgo.getResult())

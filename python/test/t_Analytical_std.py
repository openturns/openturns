#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# We create a numerical math function
myFunction = ot.SymbolicFunction(["E", "F", "L", "I"], ["-F*L^3/(3*E*I)"])

dim = myFunction.getInputDimension()
# We create a normal distribution point of dimension 1
mean = ot.Point(dim, 0.0)
mean[0] = 50.0  # E
mean[1] = 1.0  # F
mean[2] = 10.0  # L
mean[3] = 5.0  # I
sigma = ot.Point(dim, 1.0)
R = ot.IdentityMatrix(dim)
myDistribution = ot.Normal(mean, sigma, R)

# We create a 'usual' RandomVector from the Distribution
vect = ot.RandomVector(myDistribution)

# We create a composite random vector
output = ot.CompositeRandomVector(myFunction, vect)

# We create an Event from this RandomVector
myEvent = ot.ThresholdEvent(output, ot.Less(), -3.0)

# We create an OptimizationAlgorithm algorithm
myCobyla = ot.Cobyla()
myCobyla.setMaximumCallsNumber(400)
myCobyla.setMaximumAbsoluteError(1.0e-10)
myCobyla.setMaximumRelativeError(1.0e-10)
myCobyla.setMaximumResidualError(1.0e-10)
myCobyla.setMaximumConstraintError(1.0e-10)
print("myCobyla=", myCobyla)

# We create a Analytical algorithm
# The first parameter is an OptimizationAlgorithm
# The second parameter is an event
# The third parameter is a starting point for the design point research
myAlgo = ot.Analytical(myCobyla, myEvent, mean)

print("Analytical=", myAlgo)
print("physical starting point=", myAlgo.getPhysicalStartingPoint())
print("event=", myAlgo.getEvent())
print("nearest point algorithm=", myAlgo.getNearestPointAlgorithm())
print("result=", myAlgo.getAnalyticalResult())
myAlgo.run()
print("result=", myAlgo.getAnalyticalResult())

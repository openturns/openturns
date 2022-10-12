#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# We create a numerical math function
myFunction = ot.SymbolicFunction(("E", "F", "L", "I"), ("-F*L^3/(3.*E*I)",))
myFunction = ot.MemoizeFunction(myFunction)
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
myEvent = ot.ThresholdEvent(output, ot.Less(), -3.0)

# We create a Monte Carlo algorithm
experiment = ot.MonteCarloExperiment()
myAlgo = ot.ProbabilitySimulationAlgorithm(myEvent, experiment)
myAlgo.setMaximumOuterSampling(500)
myAlgo.setBlockSize(10)
myAlgo.setMaximumCoefficientOfVariation(0.05)

print("MonteCarlo=", myAlgo)

# Perform the simulation
myAlgo.run()

# Stream out the result
result = myAlgo.getResult()
print("MonteCarlo result=", result)

# Compute sensitivity information
print("mean point in event domain=", result.getMeanPointInEventDomain())
print("importance factors=", result.getImportanceFactors())

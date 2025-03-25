#! /usr/bin/env python

import openturns as ot


ot.TESTPREAMBLE()

# We create a numerical math function
myFunction = ot.SymbolicFunction(["x0", "x1"], ["-(6+x0^2-x1)"])

dim = myFunction.getInputDimension()
# We create a normal distribution point of dimension 1
mean = [5.0, 2.1]
sigma = [3.3, 3.0]
R = ot.CorrelationMatrix(dim)
myDistribution = ot.Normal(mean, sigma, R)
myDistribution.setDescription(["Marginal 1", "Marginal 2"])

# We create a 'usual' RandomVector from the Distribution
vect = ot.RandomVector(myDistribution)

# We create a composite random vector
output = ot.CompositeRandomVector(myFunction, vect)
output.setDescription(["Interest Variable 1"])

# We create an Event from this RandomVector
myEvent = ot.ThresholdEvent(output, ot.Greater(), 0.0)

# We create a NearestPoint algorithm
myCobyla = ot.Cobyla()
myCobyla.setStartingPoint(mean)
myCobyla.setMaximumCallsNumber(200)
myCobyla.setMaximumAbsoluteError(1.0e-10)
myCobyla.setMaximumRelativeError(1.0e-10)
myCobyla.setMaximumResidualError(1.0e-10)
myCobyla.setMaximumConstraintError(1.0e-10)
print("myCobyla=", myCobyla)

# We create a FORM algorithm
# The first parameter is an OptimizationAlgorithm
# The second parameter is an event
myAlgo = ot.FORM(myCobyla, myEvent)

print("FORM=", myAlgo)

# Perform the simulation
myAlgo.run()

# Stream out the result
result = ot.FORMResult(myAlgo.getResult())
print("importance factors=", result.getImportanceFactors())

# Graph 1 : Importance Factors graph
importanceFactorsGraph = result.drawImportanceFactors()

# Graph 2 : Hasofer Reliability Index Sensitivity Graphs graph
reliabilityIndexSensitivityGraphs = result.drawHasoferReliabilityIndexSensitivity()
graph2a = reliabilityIndexSensitivityGraphs[0]

graph2b = reliabilityIndexSensitivityGraphs[1]

# Graph 3 : FORM Event Probability Sensitivity Graphs graph
eventProbabilitySensitivityGraphs = result.drawEventProbabilitySensitivity()
graph3a = eventProbabilitySensitivityGraphs[0]

graph3b = eventProbabilitySensitivityGraphs[1]

# Graph 4 : Convergence history
graph4 = result.getOptimizationResult().drawErrorHistory()

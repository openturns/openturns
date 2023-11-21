#! /usr/bin/env python

import openturns as ot
import math as m

ot.TESTPREAMBLE()


def printPoint(point, digits):
    oss = "["
    eps = pow(0.1, digits)
    for i in range(point.getDimension()):
        if i == 0:
            sep = ""
        else:
            sep = ","
        if m.fabs(point[i]) < eps:
            oss += sep + "%.6f" % m.fabs(point[i])
        else:
            oss += sep + "%.6f" % point[i]
        sep = ","
    oss += "]"
    return oss


# We create a numerical math function
myFunction = ot.SymbolicFunction(["E", "F", "L", "I"], ["-F*L^3/(3*E*I)"])

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
output = ot.CompositeRandomVector(myFunction, vect)

# We create an Event from this RandomVector
myEvent = ot.ThresholdEvent(output, ot.Less(), -3.0)

# We create a NearestPoint algorithm
myCobyla = ot.Cobyla()
myCobyla.setMaximumCallsNumber(400)
myCobyla.setMaximumAbsoluteError(1.0e-10)
myCobyla.setMaximumRelativeError(1.0e-10)
myCobyla.setMaximumResidualError(1.0e-10)
myCobyla.setMaximumConstraintError(1.0e-10)
print("myCobyla=", myCobyla)

# We create a FORM algorithm
# The first parameter is an OptimizationAlgorithm
# The second parameter is an event
# The third parameter is a starting point for the design point research
myAlgo = ot.FORM(myCobyla, myEvent, mean)

print("FORM=", myAlgo)

# Perform the simulation
myAlgo.run()

# Stream out the iresult
result = myAlgo.getResult()
digits = 5
print("event probability=%.6f" % result.getEventProbability())
print("generalized reliability index=%.6f" % result.getGeneralisedReliabilityIndex())
print(
    "standard space design point=",
    printPoint(result.getStandardSpaceDesignPoint(), digits),
)
print(
    "physical space design point=",
    printPoint(result.getPhysicalSpaceDesignPoint(), digits),
)

# Is the standard point origin in failure space?
print(
    "is standard point origin in failure space? %s"
    % (result.getIsStandardPointOriginInFailureSpace() and "true" or "false")
)

print("importance factors=", printPoint(result.getImportanceFactors(), digits))
print("Hasofer reliability index=%.6f" % result.getHasoferReliabilityIndex())

# run twice
f = ot.SymbolicFunction(["x"], ["x-1.25"])
dist = ot.Normal(2.0, 0.5)
vect = ot.RandomVector(dist)
output = ot.CompositeRandomVector(f, vect)
event = ot.ThresholdEvent(output, ot.Less(), 0.0)
solver = ot.Cobyla()
# -------------------------------------------------------
algo = ot.FORM(solver, event, dist.getMean())
algo.run()
result = algo.getResult()
# -------------------------------------------------------
algo_2 = ot.FORM(solver, event, dist.getMean())
algo_2.run()
result_2 = algo_2.getResult()
assert result.getEventProbability() == result_2.getEventProbability(), "wrong pf"

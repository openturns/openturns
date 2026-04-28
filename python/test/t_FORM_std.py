#! /usr/bin/env python

import openturns as ot
from openturns.testing import assert_almost_equal

ot.TESTPREAMBLE()
ot.PlatformInfo.SetNumericalPrecision(5)

# We create a numerical math function
myFunction = ot.SymbolicFunction(["E", "F", "L", "I"], ["-F*L^3/(3*E*I)"])

dim = myFunction.getInputDimension()

# We create a normal distribution point of dimension 1
mean = [50.0, 1.0, 10.0, 5.0]
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
myCobyla.setStartingPoint(mean)
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
myAlgo = ot.FORM(myCobyla, myEvent)

print("FORM=", myAlgo)

# Perform the simulation
myAlgo.run()

# Stream out the iresult
result = myAlgo.getResult()
print("event probability=%.6f" % result.getEventProbability())
print("generalized reliability index=%.6f" % result.getGeneralisedReliabilityIndex())
print(
    "standard space design point=",
    result.getStandardSpaceDesignPoint(),
)
print(
    "physical space design point=",
    result.getPhysicalSpaceDesignPoint(),
)

# Is the standard point origin in failure space?
print(
    "is standard point origin in failure space? %s"
    % (result.getIsStandardPointOriginInFailureSpace() and "true" or "false")
)

print("importance factors=", result.getImportanceFactors())
print("Hasofer reliability index=%.6f" % result.getHasoferReliabilityIndex())

# Test with MultiStart algorithm
ot.RandomGenerator.SetSeed(0)

startingSample = myDistribution.getSample(10)

myMultiStartCobyla = ot.MultiStart(ot.Cobyla(), startingSample)
myMultiStartCobyla.setMaximumCallsNumber(400)
myMultiStartCobyla.setMaximumAbsoluteError(1.0e-5)
myMultiStartCobyla.setMaximumRelativeError(1.0e-5)
myMultiStartCobyla.setMaximumResidualError(1.0e-5)
myMultiStartCobyla.setMaximumConstraintError(1.0e-5)

# We create a FORM algorithm with MultiStart
algo_multistart = ot.FORM(myMultiStartCobyla, myEvent)

# Perform the simulation
algo_multistart.run()
result_multistart = algo_multistart.getResult()
assert_almost_equal(
    result_multistart.getEventProbability(), result.getEventProbability(), 1e-4, 0.0
)

# run twice
f = ot.SymbolicFunction(["x"], ["x-1.25"])
dist = ot.Normal(2.0, 0.5)
vect = ot.RandomVector(dist)
output = ot.CompositeRandomVector(f, vect)
event = ot.ThresholdEvent(output, ot.Less(), 0.0)
solver = ot.AbdoRackwitz()
solver.setStartingPoint(dist.getMean())
# -------------------------------------------------------
algo = ot.FORM(solver, event)
algo.run()
result = algo.getResult()
# -------------------------------------------------------
algo_2 = ot.FORM(solver, event)
algo_2.run()
result_2 = algo_2.getResult()
assert result.getEventProbability() == result_2.getEventProbability(), "wrong pf"

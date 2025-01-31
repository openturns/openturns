#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


# We create a numerical math function
myFunction = ot.SymbolicFunction(["E", "F", "L", "I"], ["-F*L^3/(3*E*I)"])

dim = myFunction.getInputDimension()
# We create a normal distribution point of dimension 1
mean = [50.0, 1.0, 10.0, 5.0]
sigma = ot.Point(dim, 1.0)
R = ot.IdentityMatrix(dim)
myDistribution = ot.Normal(mean, sigma, R)

# We create a 'usual' RandomVector from the Distribution
vect = ot.RandomVector(myDistribution)

# We create a composite random vector
output = ot.CompositeRandomVector(myFunction, vect)

# We create an Event from this RandomVector
myEvent = ot.ThresholdEvent(output, ot.Less(), -3.0)

# We create an AnalyticalResult based on fictive results
result = ot.AnalyticalResult(sigma, myEvent, False)

print("result=", result)
print(
    "standard space design point=",
    result.getStandardSpaceDesignPoint()
)
print(
    "physical space design point=",
    result.getPhysicalSpaceDesignPoint()
)
print(
    "is standard point origin in failure space? ",
    result.getIsStandardPointOriginInFailureSpace(),
)
print("importance factors=", result.getImportanceFactors())
print(
    "importance factors(classical)=",
    result.getImportanceFactors(ot.AnalyticalResult.CLASSICAL)
)
print(
    "importance factors(physical) =",
    result.getImportanceFactors(ot.AnalyticalResult.PHYSICAL)
)
print("Hasofer reliability index=%.5f" % result.getHasoferReliabilityIndex())
print("graph importance factors=", result.drawImportanceFactors())
print("graph sensitivity=", result.drawHasoferReliabilityIndexSensitivity())

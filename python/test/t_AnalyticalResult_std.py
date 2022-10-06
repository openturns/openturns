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
            oss += sep + '%.6f' % m.fabs(point[i])
        else:
            oss += sep + '%.6f' % point[i]
        sep = ","
    oss += "]"
    return oss



# We create a numerical math function
myFunction = ot.SymbolicFunction(
    ["E", "F", "L", "I"], ["-F*L^3/(3*E*I)"])

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

# We create an AnalyticalResult based on fictive results
result = ot.AnalyticalResult(sigma, myEvent, False)

print("result=", result)

digits = 5
print("standard space design point=", printPoint(
    result.getStandardSpaceDesignPoint(), digits))
print("physical space design point=", printPoint(
    result.getPhysicalSpaceDesignPoint(), digits))
print("is standard point origin in failure space? ",
      result.getIsStandardPointOriginInFailureSpace())
print("importance factors=", printPoint(
    result.getImportanceFactors(), digits))
print("importance factors(classical)=", printPoint(
    result.getImportanceFactors(ot.AnalyticalResult.CLASSICAL), digits))
print("importance factors(physical) =", printPoint(
    result.getImportanceFactors(ot.AnalyticalResult.PHYSICAL), digits))
print("Hasofer reliability index=%.5f" %
      result.getHasoferReliabilityIndex())
print("graph importance factors=", result.drawImportanceFactors())
print("graph sensitivity=",
      result.drawHasoferReliabilityIndexSensitivity())

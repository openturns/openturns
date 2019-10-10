#! /usr/bin/env python

from __future__ import print_function
from openturns import *

from math import *

TESTPREAMBLE()


def printPoint(point, digits):
    oss = "["
    eps = pow(0.1, digits)
    for i in range(point.getDimension()):
        if i == 0:
            sep = ""
        else:
            sep = ","
        if fabs(point[i]) < eps:
            oss += sep + '%.6f' % fabs(point[i])
        else:
            oss += sep + '%.6f' % point[i]
        sep = ","
    oss += "]"
    return oss


try:
    # We create a numerical math function
    myFunction = SymbolicFunction(
        ["E", "F", "L", "I"], ["-F*L^3/(3*E*I)"])

    dim = myFunction.getInputDimension()
    # We create a normal distribution point of dimension 1
    mean = Point(dim, 0.0)
    mean[0] = 50.0  # E
    mean[1] = 1.0  # F
    mean[2] = 10.0  # L
    mean[3] = 5.0  # I
    sigma = Point(dim, 1.0)
    R = IdentityMatrix(dim)
    myDistribution = Normal(mean, sigma, R)

    # We create a 'usual' RandomVector from the Distribution
    vect = RandomVector(myDistribution)

    # We create a composite random vector
    output = CompositeRandomVector(myFunction, vect)

    # We create an Event from this RandomVector
    myEvent = ThresholdEvent(output, Less(), -3.0)

    # We create an AnalyticalResult based on fictive results
    result = AnalyticalResult(sigma, myEvent, False)

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
        result.getImportanceFactors(AnalyticalResult.CLASSICAL), digits))
    print("importance factors(physical) =", printPoint(
        result.getImportanceFactors(AnalyticalResult.PHYSICAL), digits))
    print("Hasofer reliability index=%.5f" %
          result.getHasoferReliabilityIndex())
    print("graph importance factors=", result.drawImportanceFactors())
    print("graph sensitivity=",
          result.drawHasoferReliabilityIndexSensitivity())
except:
    import sys
    print("t_AnalyticalResult_std.py", sys.exc_info()[0], sys.exc_info()[1])

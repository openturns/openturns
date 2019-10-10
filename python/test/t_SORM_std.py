#! /usr/bin/env python

from __future__ import print_function
from openturns import *

from math import *


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


TESTPREAMBLE()

try:

    # We create a numerical math function
    myFunction = SymbolicFunction(
        ["E", "F", "L", "I"], ["-F*L^3/(3*E*I)"])

    dim = myFunction.getInputDimension()

    # We create a normal distribution point of dimension 1
    mean = Point(dim, 0.0)
    # E
    mean[0] = 50.0
    # F
    mean[1] = 1.0
    # L
    mean[2] = 10.0
    # I
    mean[3] = 5.0
    sigma = Point(dim, 1.0)
    R = IdentityMatrix(dim)
    myDistribution = Normal(mean, sigma, R)

    # We create a 'usual' RandomVector from the Distribution
    vect = RandomVector(myDistribution)

    # We create a composite random vector
    output = CompositeRandomVector(myFunction, vect)

    # We create an Event from this RandomVector
    myEvent = ThresholdEvent(output, Less(), -1.5)

    # We create a NearestPoint algorithm
    myAbdoRackwitz = AbdoRackwitz()
    myAbdoRackwitz.setMaximumIterationNumber(100)
    myAbdoRackwitz.setMaximumAbsoluteError(1.0e-10)
    myAbdoRackwitz.setMaximumRelativeError(1.0e-10)
    myAbdoRackwitz.setMaximumResidualError(1.0e-10)
    myAbdoRackwitz.setMaximumConstraintError(1.0e-10)
    print("myAbdoRackwitz=", myAbdoRackwitz)

    # We create a FORM algorithm
    # The first parameter is an OptimizationAlgorithm
    # The second parameter is an event
    # The third parameter is a starting point for the design point research
    myAlgo = SORM(myAbdoRackwitz, myEvent, mean)

    # Perform the simulation
    myAlgo.run()

    # Stream out the result
    result = myAlgo.getResult()
    digits = 5
    print("Breitung event probability=%.6f" %
          result.getEventProbabilityBreitung())
    print("Breitung generalized reliability index=%.6f" %
          result.getGeneralisedReliabilityIndexBreitung())
    print("HohenBichler event probability=%.6f" %
          result.getEventProbabilityHohenBichler())
    print("HohenBichler generalized reliability index=%.6f" %
          result.getGeneralisedReliabilityIndexHohenBichler())
    print("Tvedt event probability=%.6f" % result.getEventProbabilityTvedt())
    print("Tvedt generalized reliability index=%.6f" %
          result.getGeneralisedReliabilityIndexTvedt())
    print("sorted curvatures=", printPoint(
        result.getSortedCurvatures(), digits))
    print("standard space design point=", printPoint(
        result.getStandardSpaceDesignPoint(), digits))
    print("physical space design point=", printPoint(
        result.getPhysicalSpaceDesignPoint(), digits))

    # Is the standard point origin in failure space?
    print("is standard point origin in failure space? ",
          result.getIsStandardPointOriginInFailureSpace())

    print("importance factors=", printPoint(
        result.getImportanceFactors(), digits))
    print("Hasofer reliability index=%.6f" %
          result.getHasoferReliabilityIndex())

except:
    import sys
    print("t_SORM_std.py", sys.exc_info()[0], sys.exc_info()[1])

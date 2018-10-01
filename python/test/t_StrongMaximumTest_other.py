#! /usr/bin/env python

from __future__ import print_function
from openturns import *
from math import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)


def printSample(name, sample):
    oss = name + "=["
    for counter in range(sample.getSize()):
        if (counter != 0):
            oss += ";"
        oss += "["
        point = sample[counter]
        for coordinate in range(point.getDimension()):
            if (coordinate != 0):
                oss += ","
            oss += "%.6f" % point[coordinate]
        oss += "]"
    oss += "]"
    return oss


try:

    # We create a numerical math function
    myFunction = SymbolicFunction(
        ["x1", "x2"], ["(x1-0.6)*(x1-0.6)-x2*x2"])

    dim = myFunction.getInputDimension()
    # We create a normal distribution point of dimension 1
    mean = Point(dim, 0.0)
    sigma = Point(dim, 1.0)
    R = IdentityMatrix(dim)
    myDistribution = Normal(mean, sigma, R)

    # We create a 'usual' RandomVector from the Distribution
    vect = RandomVector(myDistribution)

    # We create a composite random vector
    output = CompositeRandomVector(myFunction, vect)

    # We create a StandardEvent from this RandomVector  : RandomVector > 4.0
    seuil = 40
    myStandardEvent = StandardEvent(
        output, Greater(), seuil)

    # We create the design point
    designPoint = Point(dim, 0.0)
    C = 0.6
    designPoint[0] = - sqrt(seuil) + C

    # We create the "second" design point
    pseudoDesignPoint = Point(dim, 0.0)
    pseudoDesignPoint[0] = sqrt(seuil) + C

    importanceLevel = 0.01
    accuracyLevel = 2
    confidenceLevel = 0.999

    myTest = StrongMaximumTest(
        myStandardEvent, designPoint, importanceLevel, accuracyLevel, confidenceLevel)
    print("myTest=", myTest)
    myTest.run()
    print("Beta = %.6f" % designPoint.norm())
    print("Discretised sphere radius = %6f" % (
        designPoint.norm() * (1 + myTest.getAccuracyLevel() * myTest.getDeltaEpsilon())))
    print("PointNumber = ", myTest.getPointNumber())
    print("DesignPointVicinity Angle (rad)= %.6f" %
          acos(myTest.getDesignPointVicinity()))
    print("DesignPointVicinity Angle (deg)= %.6f" %
          (acos(myTest.getDesignPointVicinity()) * 180 / pi))
    print("Near Design Point Verifying Event Points Number = ", (
        myTest.getNearDesignPointVerifyingEventPoints()).getSize())
    print("Near Design Point Violating Event Points Number = ", (
        myTest.getNearDesignPointViolatingEventPoints()).getSize())
    print("Far Design Point Verifying Event Points Number = ", (
        myTest.getFarDesignPointVerifyingEventPoints()).getSize())
    print("Far Design Point Violating Event Points Number = ", (
        myTest.getFarDesignPointViolatingEventPoints()).getSize())
    print("//////")

    # parameters of the test
    print("importanceLevel=%.6f" % myTest.getImportanceLevel())
    print("accuracyLevel=%.6f" % myTest.getAccuracyLevel())
    print("confidenceLevel=%.6f" % myTest.getConfidenceLevel())

    # design point coordinates
    print("standardSpaceDesignPoint=[%.6f" % myTest.getStandardSpaceDesignPoint()[
          0], ",%.6f" % myTest.getStandardSpaceDesignPoint()[1], "]")

    # pseudo design point coordinates
    print("pseudoStandardSpaceDesignPoint=[%.6f" % pseudoDesignPoint[
          0], ",%.6f" % pseudoDesignPoint[1], "]")

    # cout of the coordinates of the points of the 4 samples
    # NearDesignPointVerifyingEventPoints
    print(printSample("NearDesignPointVerifyingEventPointsSample",
                      myTest.getNearDesignPointVerifyingEventPoints()))
    # NearDesignPointViolatingEventPoints
    print(printSample("NearDesignPointViolatingEventPoints",
                      myTest.getNearDesignPointViolatingEventPoints()))
    # FarDesignPointVerifyingEventPoints
    print(printSample("FarDesignPointVerifyingEventPoints",
                      myTest.getFarDesignPointVerifyingEventPoints()))
    # FarDesignPointViolatingEventPoints
    print(printSample("FarDesignPointViolatingEventPoints",
                      myTest.getFarDesignPointViolatingEventPoints()))

    #    print  "myTest (after run())=" , myTest

except:
    import sys
    print("t_strongMaxTest_other.py", sys.exc_info()[0], sys.exc_info()[1])

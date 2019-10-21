#! /usr/bin/env python

from __future__ import print_function
from openturns import *

from math import *
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
        # Analytical construction
    inputFunc = Description(2)
    inputFunc[0] = "x0"
    inputFunc[1] = "x1"
    formulas = Description(1)
    formulas[0] = "-(6+x0^2-x1)"
    print("formulas=", formulas)
    myFunction = SymbolicFunction(inputFunc, formulas)

    dim = myFunction.getInputDimension()
    # We create a normal distribution point of dimension 1
    mean = Point(dim, 0.0)
# x0
    mean[0] = 5.0
# x1
    mean[1] = 2.1
    sigma = Point(dim, 0.0)
# x0
    sigma[0] = 3.3
# x1
    sigma[1] = 3.0
    R = IdentityMatrix(dim)

#
    testDistributions = DistributionCollection(2)
    testDistributions[0] = Normal(mean, sigma, R)
    marginals = DistributionCollection(2)
    marginals[0] = testDistributions[0].getMarginal(0)
    marginals[1] = testDistributions[0].getMarginal(1)
    testDistributions[1] = ComposedDistribution(marginals, NormalCopula(R))
    for i in range(1):

        myDistribution = testDistributions[i]
        # We name the components of the distribution
        componentDescription = Description(dim)
        componentDescription[0] = "Marginal 1"
        componentDescription[1] = "Marginal 2"
        myDistribution.setDescription(componentDescription)

        # We create a 'usual' RandomVector from the Distribution
        vect = RandomVector(myDistribution)

        # We create a composite random vector
        output = CompositeRandomVector(myFunction, vect)
        outputDescription = Description(1)
        outputDescription[0] = "Interest Variable 1"
        output.setDescription(outputDescription)

        # We create an Event from this RandomVector
        myEvent = ThresholdEvent(output, Greater(), 0.0)

        # We create a NearestPoint algorithm
        myCobyla = Cobyla()
        myCobyla.setMaximumEvaluationNumber(200)
        myCobyla.setMaximumAbsoluteError(1.0e-10)
        myCobyla.setMaximumRelativeError(1.0e-10)
        myCobyla.setMaximumResidualError(1.0e-10)
        myCobyla.setMaximumConstraintError(1.0e-10)
        print("myCobyla=", myCobyla)

        # We create a FORM algorithm
        # The first parameter is an OptimizationAlgorithm
        # The second parameter is an event
        # The third parameter is a starting point for the design point research
        myAlgo = FORM(myCobyla, myEvent, mean)

        print("FORM=", myAlgo)

        # Perform the simulation
        myAlgo.run()

        # Stream out the result
        result = FORMResult(myAlgo.getResult())
        digits = 5
        print("importance factors=", printPoint(
            result.getImportanceFactors(), digits))
        print("Hasofer reliability index=%.6f" %
              result.getHasoferReliabilityIndex())
        print("result=", result)

        # Hasofer Reliability Index Sensitivity
        hasoferReliabilityIndexSensitivity = result.getHasoferReliabilityIndexSensitivity(
        )
        print("hasoferReliabilityIndexSensitivity = ",
              repr(hasoferReliabilityIndexSensitivity))

        # Event Probability Sensitivity
        eventProbabilitySensitivity = result.getEventProbabilitySensitivity()
        print("eventProbabilitySensitivity = ",
              repr(eventProbabilitySensitivity))

except:
    import sys
    print("t_FORM_sensitivity.py", sys.exc_info()[0], sys.exc_info()[1])

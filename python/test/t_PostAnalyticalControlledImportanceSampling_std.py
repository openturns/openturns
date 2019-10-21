#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:

    # We create a numerical math function
    myFunction = SymbolicFunction(
        ('E', 'F', 'L', 'I'), ('-F*L^3/(3.*E*I)',))

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
    myEvent = ThresholdEvent(output, Less(), -3)

    # We create a FORM algorithm
    # The first parameter is an OptimizationAlgorithm
    # The second parameter is an event
    # The third parameter is a starting point for the design point research
    myCobyla = Cobyla()
    myCobyla.setMaximumEvaluationNumber(400)
    myAlgo = FORM(myCobyla, myEvent, mean)

    # Perform the simulation
    myAlgo.run()

    # Create a PostAnalyticalControlledImportanceSampling algorithm based on
    # the previous FORM result
    formResult = myAlgo.getResult()
    mySamplingAlgo = PostAnalyticalControlledImportanceSampling(formResult)
    print("FORM probability= %.11g" % formResult.getEventProbability())
    mySamplingAlgo.setMaximumOuterSampling(250)
    mySamplingAlgo.setBlockSize(4)
    mySamplingAlgo.setMaximumCoefficientOfVariation(0.1)

    print("PostAnalyticalControlledImportanceSampling=", mySamplingAlgo)

    mySamplingAlgo.run()

    # Stream out the result
    print("PostAnalyticalControlledImportanceSampling result=",
          mySamplingAlgo.getResult())

except:
    import sys
    print("t_PostAnalyticalControlledImportanceSampling_std.py",
          sys.exc_info()[0], sys.exc_info()[1])

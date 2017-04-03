#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:

    # We create a numerical math function
    myFunction = Function(
        ('E', 'F', 'L', 'I'), ('d',), ('-F*L^3/(3.*E*I)',))

    dim = myFunction.getInputDimension()
    # We create a normal distribution point of dimension 1
    mean = NumericalPoint(dim, 0.0)
    # E
    mean[0] = 50.0
    # F
    mean[1] = 1.0
    # L
    mean[2] = 10.0
    # I
    mean[3] = 5.0
    sigma = NumericalPoint(dim, 1.0)
    R = IdentityMatrix(dim)
    myDistribution = Normal(mean, sigma, R)

    # We create a 'usual' RandomVector from the Distribution
    vect = RandomVector(myDistribution)

    # We create a composite random vector */
    output = RandomVector(myFunction, vect)

    # We create an Event from this RandomVector */
    myEvent = Event(output, Less(), -3)

# TEST ONE : directional sampling from an event (slow and safe strategy by default)
    # We create a Directional Sampling algorithm */
    myAlgo = DirectionalSampling(myEvent)
    myAlgo.setMaximumOuterSampling(250)
    myAlgo.setBlockSize(4)
    myAlgo.setMaximumCoefficientOfVariation(0.1)

    print("DirectionalSampling=", myAlgo)

    # Perform the simulation */
    myAlgo.run()

    # Stream out the result */
    print("DirectionalSampling result=", myAlgo.getResult())

# TEST TWO : diectional sampling from an event and a root strategy : Risky And Fast, and a directional sampling strategy :
    # We create a Directional Sampling algorithm */
    myAlgo2 = DirectionalSampling(myEvent, MediumSafe(), OrthogonalDirection())
    myAlgo2.setMaximumOuterSampling(250)
    myAlgo2.setBlockSize(4)
    myAlgo2.setMaximumCoefficientOfVariation(0.1)

    print("DirectionalSampling=", myAlgo2)

    # Perform the simulation */
    myAlgo2.run()

    # Stream out the result */
    print("DirectionalSampling result=", myAlgo2.getResult())

except:
    import sys
    print("t_DirectionalSampling.py", sys.exc_info()[0], sys.exc_info()[1])

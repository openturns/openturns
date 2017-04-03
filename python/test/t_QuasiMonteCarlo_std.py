#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

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

    # We create a composite random vector
    output = RandomVector(myFunction, vect)

    # We create an Event from this RandomVector
    myEvent = Event(output, Less(), -3.0)

    # We create a Monte Carlo algorithm
    myAlgo = QuasiMonteCarlo(myEvent)
    myAlgo.setMaximumOuterSampling(250)
    myAlgo.setBlockSize(4)

    print("QuasiMonteCarlo=", myAlgo)

    # Perform the simulation
    myAlgo.run()

    # Stream out the result
    print("QuasiMonteCarlo result=", myAlgo.getResult())

except:
    import sys
    print("t_QuasiMonteCarlo_std.py", sys.exc_info()[0], sys.exc_info()[1])

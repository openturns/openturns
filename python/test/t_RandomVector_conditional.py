#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    # We create a distribution
    distribution = Normal()

    print("distribution = ", repr(distribution))

    aCollection = DistributionCollection(0)
    aCollection.add(Normal(0.0, 1.0))
    aCollection.add(Uniform(1.0, 1.5))
    distributionParameters = ComposedDistribution(aCollection)
    randomParameters = RandomVector(distributionParameters)

    print("random parameters=", randomParameters)

    # We create a distribution-based conditional RandomVector
    vect = ConditionalRandomVector(distribution, randomParameters)
    print("vect=", vect)

    # Check standard methods of class RandomVector
    print("vect dimension=", vect.getDimension())
    p = NumericalPoint()
    r = vect.getRealization(p)
    print("vect realization=", repr(r))
    print("parameters value=", repr(p))
    distribution.setParametersCollection(p)
    RandomGenerator.SetSeed(0)
    # Generate a parameter set to put the random generator into the proper
    # state
    randomParameters.getRealization()
    # The realization of the distribution should be equal to the realization
    # of the conditional vector
    print("dist realization=", repr(distribution.getRealization()))

    print("vect sample =", repr(vect.getSample(5)))

except:
    import sys
    print("t_RandomVector_conditional.py",
          sys.exc_info()[0], sys.exc_info()[1])

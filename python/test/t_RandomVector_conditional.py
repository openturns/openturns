#! /usr/bin/env python

from __future__ import print_function
import openturns as ot


# We create a distribution
distribution = ot.Normal()

print("distribution = ", repr(distribution))

aCollection = ot.DistributionCollection(0)
aCollection.add(ot.Normal(0.0, 1.0))
aCollection.add(ot.Uniform(1.0, 1.5))
distributionParameters = ot.ComposedDistribution(aCollection)
randomParameters = ot.RandomVector(distributionParameters)

print("random parameters=", randomParameters)

# We create a distribution-based conditional RandomVector
vect = ot.ConditionalRandomVector(distribution, randomParameters)
print("vect=", vect)

# Check standard methods of class RandomVector
print("vect dimension=", vect.getDimension())
p = ot.NumericalPoint()
r = vect.getRealization(p)
print("vect realization=", repr(r))
print("parameters value=", repr(p))
distribution.setParameters(p)
ot.RandomGenerator.SetSeed(0)
# Generate a parameter set to put the random generator into the proper
# state
randomParameters.getRealization()
# The realization of the distribution should be equal to the realization
# of the conditional vector
print("dist realization=", repr(distribution.getRealization()))

print("vect sample =", repr(vect.getSample(5)))

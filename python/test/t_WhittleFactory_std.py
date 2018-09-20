#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)


# Reduce the precision output as the estimation is based on a lazy
# optimizer
PlatformInfo.SetNumericalPrecision(4)

# ARMA(p, q)
p = 1
q = 1

# ARMACoefficients initializing
arCoefficients = Point(p, 0.80)
maCoefficients = Point(q, 0.50)

# ARMA creation
myARMA = ARMA(ARMACoefficients(arCoefficients),
              ARMACoefficients(maCoefficients), WhiteNoise(Normal(0.0, 0.05)))
myARMA.setTimeGrid(RegularGrid(0.0, 0.1, 256))
print("myARMA process=", myARMA)

# Create a realization
timeSeries = myARMA.getRealization()

# Create a sample
sample = myARMA.getSample(100)

# First, build an ARMA based on a given order using the WhittleFactory
factory = WhittleFactory(p, q)
# factory.setVerbose(False)
print("factory=", factory)
print("factory as an ARMA factory=", ARMAFactory(factory))
informationCriteria = Point()
result, informationCriteria = factory.buildWithCriteria(TimeSeries(timeSeries))
# print "Estimated ARMA=", result
# print "Information criteria=", informationCriteria
result2, informationCriteria = factory.buildWithCriteria(sample)
# print "Estimated ARMA=", result2
# print "Information criteria=", informationCriteria

# Second, build the best ARMA based on a given range of order using the
# WhittleFactory
pIndices = Indices(p + 1)
pIndices.fill()
qIndices = Indices(q + 1)
qIndices.fill()
factory = WhittleFactory(pIndices, qIndices)
print("factory=", factory)
informationCriteria = Point()
result, informationCriteria = factory.buildWithCriteria(TimeSeries(timeSeries))
# print "Estimated ARMA=", result
# print "Information criteria=", informationCriteria
# print "History=", factory.getHistory()
result2, informationCriteria = factory.buildWithCriteria(sample)
# print "Estimated ARMA=", result2
# print "Information criteria=", informationCriteria
history = WhittleFactoryStateCollection(factory.getHistory())
firstTestes = WhittleFactoryState(history[0])
# print "History=", factory.getHistory()

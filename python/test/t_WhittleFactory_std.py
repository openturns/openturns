#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


# Reduce the precision output as the estimation is based on a lazy
# optimizer
ot.PlatformInfo.SetNumericalPrecision(4)

# ARMA(p, q)
p = 1
q = 1

# ARMACoefficients initializing
arCoefficients = ot.Point(p, 0.80)
maCoefficients = ot.Point(q, 0.50)

# ARMA creation
myARMA = ot.ARMA(ot.ARMACoefficients(arCoefficients),
              ot.ARMACoefficients(maCoefficients), ot.WhiteNoise(ot.Normal(0.0, 0.05)))
myARMA.setTimeGrid(ot.RegularGrid(0.0, 0.1, 256))
print("myARMA process=", myARMA)

# Create a realization
timeSeries = myARMA.getRealization()

# Create a sample
sample = myARMA.getSample(100)

# First, build an ARMA based on a given order using the WhittleFactory
factory = ot.WhittleFactory(p, q)
# factory.setVerbose(False)
print("factory=", factory)
print("factory as an ARMA factory=", ot.ARMAFactory(factory))
result, informationCriteria = factory.buildWithCriteria(ot.TimeSeries(timeSeries))
# print "Estimated ARMA=", result
# print "Information criteria=", informationCriteria
result2, informationCriteria = factory.buildWithCriteria(sample)
# print "Estimated ARMA=", result2
# print "Information criteria=", informationCriteria

# Second, build the best ARMA based on a given range of order using the
# WhittleFactory
pIndices = ot.Indices(p + 1)
pIndices.fill()
qIndices = ot.Indices(q + 1)
qIndices.fill()
factory = ot.WhittleFactory(pIndices, qIndices)
print("factory=", factory)
result, informationCriteria = factory.buildWithCriteria(ot.TimeSeries(timeSeries))
# print "Estimated ARMA=", result
# print "Information criteria=", informationCriteria
# print "History=", factory.getHistory()
result2, informationCriteria = factory.buildWithCriteria(sample)
# print "Estimated ARMA=", result2
# print "Information criteria=", informationCriteria
history = ot.WhittleFactoryStateCollection(factory.getHistory())
firstTestes = ot.WhittleFactoryState(history[0])
# print "History=", factory.getHistory()

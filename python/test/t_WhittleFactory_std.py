#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:

    # Reduce the precision output as the estimation is based on a lazy
    # optimizer
    PlatformInfo.SetNumericalPrecision(4)

    # ARMA(p, q)
    p = 1
    q = 1

    # ARMACoefficients intializing
    arCoefficients = NumericalPoint(p, 0.80)
    maCoefficients = NumericalPoint(q, 0.50)

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
    informationCriteria = NumericalPoint()
    result = factory.build(TimeSeries(timeSeries), informationCriteria)
    # print "Estimated ARMA=", result
    # print "Information criteria=", informationCriteria
    result2 = factory.build(sample, informationCriteria)
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
    informationCriteria = NumericalPoint()
    result = factory.build(TimeSeries(timeSeries), informationCriteria)
    # print "Estimated ARMA=", result
    # print "Information criteria=", informationCriteria
    # print "History=", factory.getHistory()
    result2 = factory.build(sample, informationCriteria)
    # print "Estimated ARMA=", result2
    # print "Information criteria=", informationCriteria
    history = WhittleFactoryStateCollection(factory.getHistory())
    firstTestes = WhittleFactoryState(history[0])
    # print "History=", factory.getHistory()

except:
    import sys
    print("t_WhittleFactory_std.py", sys.exc_info()[0], sys.exc_info()[1])

#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(0)

try:
    dimension = 2
    sampleSize = 1000

    # we create an analytical function
    analytical = ot.Function(['x0', 'x1'], ['f'], ['10+3*x0+x1'])

    # we create a collection of centered and reduced gaussian distributions
    aCollection = [ot.Normal()] * dimension

    # we create one distribution object
    aDistribution = ot.ComposedDistribution(aCollection)

    # Random vectors
    randomVector = ot.RandomVector(aDistribution)
    composite = ot.RandomVector(analytical, randomVector)
    print("composite=", composite)

    # we create two input samples for the function
    inputSample = randomVector.getSample(sampleSize)
    outputSample = analytical(inputSample)

    src = ot.CorrelationAnalysis.SRC(inputSample, outputSample)
    print("src=", src)
except:
    import sys
    print("t_CorrelationAnalysis_std.py",
          sys.exc_info()[0], sys.exc_info()[1])

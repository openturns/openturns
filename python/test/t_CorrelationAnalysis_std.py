#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    dimension = 2
    sampleSize = 1000

    # we create an analytical function
    analytical = NumericalMathFunction(['x0', 'x1'], ['f'], ['10+3*x0+x1'])

    # we create a collection of uniform distributions over [-Pi Pi[
    aCollection = DistributionCollection()
    for i in range(dimension):
        aCollection.add(Distribution(Normal()))

    # we create one distribution object
    aDistribution = ComposedDistribution(
        aCollection, Copula(IndependentCopula(dimension)))

    randomVector = RandomVector(aDistribution)
    composite = RandomVector(analytical, randomVector)
    print("composite=", composite)

    # we create two input samples for the function
    inputSample = randomVector.getSample(sampleSize)
    outputSample = analytical(inputSample)

    src = CorrelationAnalysis.SRC(inputSample, outputSample)
    print("src=", src)
except:
    import sys
    print("t_CorrelationAnalysis_sobol.py",
          sys.exc_info()[0], sys.exc_info()[1])

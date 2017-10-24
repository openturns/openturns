#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()

dimension = 2
sampleSize = 1000

# we create an analytical function
analytical = ot.SymbolicFunction(['x0', 'x1'], ['10+3*x0+x1'])

# we create a collection of centered and reduced gaussian distributions
aCollection = [ot.Normal()] * dimension

# we create one distribution object
aDistribution = ot.ComposedDistribution(aCollection)

# Random vectors
randomVector = ot.RandomVector(aDistribution)

# we create two input samples for the function
inputSample = randomVector.getSample(sampleSize)
outputSample = analytical(inputSample)

src = ot.CorrelationAnalysis.SRC(inputSample, outputSample)
print("src=", src)

srrc = ot.CorrelationAnalysis.SRRC(inputSample, outputSample)
print("srrc=", srrc)

pcc = ot.CorrelationAnalysis.PCC(inputSample, outputSample)
print("pcc=", pcc)

prcc = ot.CorrelationAnalysis.PRCC(inputSample, outputSample)
print("prcc=", prcc)

pearson = ot.Point(list(map(lambda x: ot.CorrelationAnalysis.PearsonCorrelation(inputSample.getMarginal(x), outputSample), range(dimension))))
print("pearson=", pearson)

spearman = ot.Point(list(map(lambda x: ot.CorrelationAnalysis.SpearmanCorrelation(inputSample.getMarginal(x), outputSample), range(dimension))))
print("spearman=", spearman)

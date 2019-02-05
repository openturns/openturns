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

# Src with normalize
src_normalize = ot.CorrelationAnalysis.SRC(inputSample, outputSample, True)
print("src with normalize=", src_normalize)

signed_src = ot.CorrelationAnalysis.SignedSRC(inputSample, outputSample)
print("signed src=", signed_src)

srrc = ot.CorrelationAnalysis.SRRC(inputSample, outputSample)
print("srrc=", srrc)

srrc_normalize = ot.CorrelationAnalysis.SRRC(inputSample, outputSample)
print("srrc with normalize=", srrc_normalize)

pcc = ot.CorrelationAnalysis.PCC(inputSample, outputSample)
print("pcc=", pcc)

prcc = ot.CorrelationAnalysis.PRCC(inputSample, outputSample)
print("prcc=", prcc)

pearson = ot.CorrelationAnalysis.PearsonCorrelation(inputSample, outputSample)
print("pearson=", pearson)

spearman = ot.CorrelationAnalysis.SpearmanCorrelation(
    inputSample, outputSample)
print("spearman=", spearman)

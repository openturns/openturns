#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

size = 100
dim = 2

sampleX = Normal(dim).getSample(size)
sampleX0 = sampleX.getMarginal(0)
sampleY = sampleX.getMarginal(1)
sampleZ = SymbolicFunction("x", "exp(x)")(sampleX0)
# Spearman Test : test if two samples have a zero rank correlation
# H0 = zero rank correlation
# Test = True <=> zero rank correlation
# p-value threshold : probability of the critical region, ie P_H0(reject H0)
# p-value : P_H0(test statistics > observed test statistics)
# Test = True <=> p-value > p-value threshold

print("Spearman=", HypothesisTest.Spearman(sampleY, sampleZ, 0.10))

# Full Spearman Test : collection of tests of zero correlation between the 1D marginals of a sample and another 1D sample
# H0 = zero rank correlation
# Test = True <=> zero rank correlation
# p-value threshold : probability of the critical region, ie P_H0(reject H0)
# p-value : P_H0(test statistics > observed test statistics)
# Test = True <=> p-value > p-value threshold

print("FullSpearmanXZ=", HypothesisTest.FullSpearman(sampleX, sampleZ, 0.10))
print("FullSpearmanYY=", HypothesisTest.FullSpearman(sampleY, sampleY, 0.10))

# Partial Spearman Test : collection of tests of zero correlation between a selection of the 1D marginals of a sample and another 1D sample
# H0 = zero rank correlation
# Test = True <=> zero rank correlation
# p-value threshold : probability of the critical region, ie P_H0(reject H0)
# p-value : P_H0(test statistics > observed test statistics)
# Test = True <=> p-value > p-value threshold

selection = [0]
# The three tests must be equal
print("PartialSpearmanX0Y=", HypothesisTest.PartialSpearman(
    sampleX, sampleY, selection, 0.10))
print("SpearmanX0Y=", HypothesisTest.Spearman(sampleX0, sampleY, 0.10))
print("FullSpearmanX0Y=", HypothesisTest.FullSpearman(sampleX0, sampleY, 0.10))

print("PartialSpearmanXY=", HypothesisTest.PartialSpearman(
    sampleX, sampleY, selection, 0.10))

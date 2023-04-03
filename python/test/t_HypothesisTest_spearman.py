#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

size = 100
dim = 2

sampleX = ot.Normal(dim).getSample(size)
sampleX0 = sampleX.getMarginal(0)
sampleY = sampleX.getMarginal(1)
sampleZ = ot.SymbolicFunction("x", "exp(x)")(sampleX0)
# Spearman Test : test if two samples have a zero rank correlation
# H0 = zero rank correlation
# Test = True <=> zero rank correlation
# p-value threshold : probability of the critical region, ie P_H0(reject H0)
# p-value : P_H0(test statistics > observed test statistics)
# Test = True <=> p-value > p-value threshold

print("Spearman=", ot.HypothesisTest.Spearman(sampleY, sampleZ, 0.10))

# Full Spearman Test : collection of tests of zero correlation between the 1D marginals of a sample and another 1D sample
# H0 = zero rank correlation
# Test = True <=> zero rank correlation
# p-value threshold : probability of the critical region, ie P_H0(reject H0)
# p-value : P_H0(test statistics > observed test statistics)
# Test = True <=> p-value > p-value threshold

# Expected result is SpecFunc::MaxScalar
fullSpearman = ot.HypothesisTest.FullSpearman(sampleX, sampleZ, 0.10)[0]
testStatistic = fullSpearman.getStatistic()
assert (testStatistic == ot.SpecFunc.MaxScalar)

fullSpearman = ot.HypothesisTest.FullSpearman(sampleX, sampleZ, 0.10)[1]
pValue = fullSpearman.getPValue()
testStatistic = fullSpearman.getStatistic()
ott.assert_almost_equal(pValue, 0.903, 1e-4, 0.0)
ott.assert_almost_equal(testStatistic, 0.1219, 1e-4, 0.0)

# Expected result is SpecFunc::MaxScalar
fullSpearman = ot.HypothesisTest.FullSpearman(sampleY, sampleY, 0.10)[0]
testStatistic = fullSpearman.getStatistic()
assert (testStatistic == ot.SpecFunc.MaxScalar)

# Partial Spearman Test : collection of tests of zero correlation between a selection of the 1D marginals of a sample and another 1D sample
# H0 = zero rank correlation
# Test = True <=> zero rank correlation
# p-value threshold : probability of the critical region, ie P_H0(reject H0)
# p-value : P_H0(test statistics > observed test statistics)
# Test = True <=> p-value > p-value threshold

selection = [0]
# The three tests must be equal
print(
    "PartialSpearmanX0Y=",
    ot.HypothesisTest.PartialSpearman(sampleX, sampleY, selection, 0.10),
)
print("SpearmanX0Y=", ot.HypothesisTest.Spearman(sampleX0, sampleY, 0.10))
print("FullSpearmanX0Y=", ot.HypothesisTest.FullSpearman(sampleX0, sampleY, 0.10))

print(
    "PartialSpearmanXY=",
    ot.HypothesisTest.PartialSpearman(sampleX, sampleY, selection, 0.10),
)

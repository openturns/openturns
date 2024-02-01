#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

dim = 10
R = ot.CorrelationMatrix(dim)
for i in range(dim):
    for j in range(i):
        R[i, j] = (i + j + 1.0) / (2.0 * dim)
mean = ot.Point(dim, 2.0)
sigma = ot.Point(dim, 3.0)
distribution = ot.Normal(mean, sigma, R)

size = 100
sample = distribution.getSample(size)
sampleX = ot.Sample(size, dim - 1)
sampleY = ot.Sample(size, 1)
for i in range(size):
    sampleY[i, 0] = sample[i, 0]
    for j in range(1, dim):
        sampleX[i, j - 1] = sample[i, j]

sampleZ = ot.Sample(size, 1)
for i in range(size):
    sampleZ[i, 0] = sampleY[i, 0] * sampleY[i, 0]

selection = ot.Indices(5)
selection.fill()
print("selection = ", selection)

# PartialPearson : Independence Pearson test between 2 samples : firstSample of dimension n and secondSample of dimension 1.
# If firstSample[i] is the numerical sample extracted from firstSample (ith coordinate of each point of the numerical sample),
# PartialPearson performs the Independence Pearson test simultaneously on firstSample[i] and secondSample, for i in the selection.
# For all i, it is supposed that the couple (firstSample[i] and secondSample) is issued from a gaussian  vector.
# Probability of the H0 reject zone : 0.10

print(
    "PartialPearsonXY=",
    ot.HypothesisTest.PartialPearson(sampleX, sampleY, selection, 0.10),
)

selection2 = ot.Indices(1, 0)
sampleX0 = ot.Sample(size, 1)
for i in range(size):
    sampleX0[i, 0] = sampleX[i, 0]

# The three tests must be equal
print(
    "PartialPearsonX0Y=",
    ot.HypothesisTest.PartialPearson(sampleX, sampleY, selection2, 0.10),
)
print("PearsonX0Y=", ot.HypothesisTest.Pearson(sampleX0, sampleY, 0.10))
print("FullPearsonX0Y=", ot.HypothesisTest.FullPearson(sampleX0, sampleY, 0.10))

# FullPearson : Independence Pearson test between 2 samples : firstSample of dimension n and secondSample of dimension 1.
# If firstSample[i] is the numerical sample extracted from firstSample (ith coordinate of each point of the numerical sample),
# FullPearson performs the Independence Pearson test simultaneously on firstSample[i] and secondSample.
# For all i, it is supposed that the couple (firstSample[i] and secondSample) is issued from a gaussian  vector.
# Probability of the H0 reject zone : 0.10
print("FullPearsonXY=", ot.HypothesisTest.FullPearson(sampleX, sampleY, 0.10))

# Expected result is SpecFunc::MaxScalar
fullPearsonYY = ot.HypothesisTest.FullPearson(sampleY, sampleY, 0.10)[0]
testStatistic = fullPearsonYY.getStatistic()
assert (testStatistic == ot.SpecFunc.MaxScalar)

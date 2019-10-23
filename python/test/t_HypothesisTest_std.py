#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(0)

try:
    dim = 10
    R = ot.CorrelationMatrix(dim)
    for i in range(dim):
        for j in range(i):
            R[i, j] = (i + j + 1.0) / (2.0 * dim)
    mean = [2.0] * dim
    sigma = [3.0] * dim
    distribution = ot.Normal(mean, sigma, R)

    size = 100
    sample = distribution.getSample(size)
    sampleY = sample.getMarginal(0)

    sampleZ = ot.Sample(size, 1)
    for i in range(size):
        sampleZ[i, 0] = sampleY[i, 0] * sampleY[i, 0]

    discreteSample1 = ot.Poisson(0.1).getSample(size)
    discreteSample2 = ot.Geometric(0.4).getSample(size)

    # ChiSquared Independance test : test if two samples (of sizes not necessarily equal) are independant ?
    # Care : discrete samples only
    # H0 = independent samples
    # p-value threshold : probability of the H0 reject zone : 0.10
    # p-value : probability (test variable decision > test variable decision evaluated on the samples)
    # Test = True <=> p-value > p-value threshold
    print("ChiSquared=", ot.HypothesisTest.ChiSquared(
        discreteSample1, discreteSample2, 0.10))
    print("ChiSquared2=", ot.HypothesisTest.ChiSquared(
        discreteSample1, discreteSample1, 0.10))

    # Pearson Test : test if two gaussian samples are independent (based on the evaluation of the linear correlation coefficient)
    # H0 : independent samples (linear correlation coefficient = 0)
    # Test = True <=> independent samples (linear correlation coefficient = 0)
    # p-value threshold : probability of the H0 reject zone : 0.10
    # p-value : probability (test variable decision > test variable decision evaluated on the samples)
    # Test = True <=> p-value > p-value threshold
    print("Pearson=", ot.HypothesisTest.Pearson(sampleY, sampleZ, 0.10))

    ot.RandomGenerator.SetSeed(0)
    sample1 = ot.Normal().getSample(20)
    sample2 = ot.Normal(0.1, 1.1).getSample(30)
    resultTwoSamplesKolmogorov = ot.HypothesisTest.TwoSamplesKolmogorov(
        sample1, sample2)
    print(resultTwoSamplesKolmogorov)
except:
    import sys
    print("t_HypothesisTest_std.py", sys.exc_info()[0], sys.exc_info()[1])

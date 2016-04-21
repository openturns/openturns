#! /usr/bin/env python

from __future__ import print_function
from openturns import *
from math import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    sample = NumericalSample(4, 3)
    sample[0] = NumericalPoint((1, 0, 9))
    sample[1] = NumericalPoint((2, 3, 5))
    sample[2] = NumericalPoint((5, 1, 8))
    sample[3] = NumericalPoint((6, 7, 2))

    print("sample=", repr(sample))
    print("min=", repr(sample.getMin()))
    print("max=", repr(sample.getMax()))
    print("mean=", repr(sample.computeMean()))
    print("covariance=", repr(sample.computeCovariance()))
    print("standard deviation=", repr(sample.computeStandardDeviation()))
    print("standard deviation per component=", repr(
        sample.computeStandardDeviationPerComponent()))
    print("Pearson correlation=", repr(sample.computePearsonCorrelation()))
    print("Spearman correlation=", repr(sample.computeSpearmanCorrelation()))
    print("Kendall tau=", repr(sample.computeKendallTau()))
    print("range per component=", repr(sample.computeRange()))
    print("median per component=", repr(sample.computeMedian()))
    print("Variance=", repr(sample.computeVariance()))
    print("Skewness=", repr(sample.computeSkewness()))
    print("Kurtosis=", repr(sample.computeKurtosis()))
    for i in range(10):
        print("Centered moment of order ", i, "=",
              sample.computeCenteredMoment(i))
    print("Marginal 1=", repr(sample.getMarginal(1)))
    indices = Indices(2)
    indices[0] = 2
    indices[1] = 0
    print("Marginal [2, 0]=", repr(sample.getMarginal(indices)))
    print("Rank=", repr(sample.rank()))
    print("Sort according to component 0=", repr(
        sample.sortAccordingToAComponent(0)))
    prob = 0.25
    print("Quantile per component(", prob, ")=", repr(
        sample.computeQuantilePerComponent(prob)))
    pointCDF = NumericalPoint(sample.getDimension(), 0.25)
    print(
        "Empirical CDF(", repr(pointCDF), "=", sample.computeEmpiricalCDF(pointCDF))
    dim = 3
    R = CorrelationMatrix(dim)
    for i in range(1, dim):
        R[i, i - 1] = 0.25
    Rtmp = CorrelationMatrix(dim)
    for i in range(dim):
        for j in range(i):
            Rtmp[i, j] = 6.0 * asin(R[i, j] / 2.0) / pi
    print("Pearson correlation (exact)=", repr(R))
    print("Spearman correlation (exact)=", repr(Rtmp))
    sample[0] = NumericalPoint((1, 0, 9))
    sample[1] = NumericalPoint((2, 3, 9))
    sample[2] = NumericalPoint((5, 1, 9))
    sample[3] = NumericalPoint((2, 2, 9))
    print("Rank=", repr(sample.rank()))
    print("Rank component 0=", repr(sample.rank(0)))
    print("Sort according to component 0=", repr(
        sample.sortAccordingToAComponent(0)))
    print("Spearman correlation=", repr(sample.computeSpearmanCorrelation()))
    print("Kendall tau=", repr(sample.computeKendallTau()))
    size = 10000
    normal = Normal(NumericalPoint(dim, 0.0), NumericalPoint(dim, 1.0), R)
    print("Normal=", repr(normal))
    print("covariance=", repr(normal.getCovariance()))
    normalSample = normal.getSample(size)
    print("Empirical covariance=", repr(normalSample.computeCovariance()))
    RPearson = normalSample.computePearsonCorrelation()
    print("Pearson correlation=", repr(RPearson))
    RSpearman = normalSample.computeSpearmanCorrelation()
    print("Spearman correlation=", repr(RSpearman))
# except TestFailed, ex :
except:
    import sys
    print("t_NumericalSample_computation.py",
          sys.exc_info()[0], sys.exc_info()[1])

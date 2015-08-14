#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    size = 100
    dim = 10
    R = CorrelationMatrix(dim)
    for i in range(dim):
        for j in range(i):
            R[i, j] = (i + j + 1.0) / (2.0 * dim)

    mean = NumericalPoint(dim, 2.0)
    sigma = NumericalPoint(dim, 3.0)
    distribution = Normal(mean, sigma, R)

    sample = distribution.getSample(size)
    sampleX = NumericalSample(size, dim - 1)
    sampleY = NumericalSample(size, 1)
    for i in range(size):
        sampleY[i] = NumericalPoint(1, sample[i, 0])
        p = NumericalPoint(dim - 1)
        for j in range(dim - 1):
            p[j] = sample[i, j + 1]
        sampleX[i] = p

    sampleZ = NumericalSample(size, 1)
    for i in range(size):
        sampleZ[i] = NumericalPoint(1, sampleY[i, 0] * sampleY[i, 0])
    print("LinearModelAdjustedRSquared=",
          LinearModelTest.LinearModelAdjustedRSquared(sampleY, sampleZ))
    print("LinearModelFisher=",
          LinearModelTest.LinearModelFisher(sampleY, sampleZ))
    print("LinearModelResidualMean=",
          LinearModelTest.LinearModelResidualMean(sampleY, sampleZ))
    print("LinearModelRSquared=",
          LinearModelTest.LinearModelRSquared(sampleY, sampleZ))

except:
    import sys
    print("t_LinearModelTest_std.py", sys.exc_info()[0], sys.exc_info()[1])

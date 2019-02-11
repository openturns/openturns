#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(0)

try:
    size = 100
    dim = 10
    R = ot.CorrelationMatrix(dim)
    for i in range(dim):
        for j in range(i):
            R[i, j] = (i + j + 1.0) / (2.0 * dim)

    mean = [2.0] * dim
    sigma = [3.0] * dim
    distribution = ot.Normal(mean, sigma, R)

    sample = distribution.getSample(size)
    sampleX = ot.Sample(size, dim - 1)
    sampleY = ot.Sample(size, 1)
    for i in range(size):
        sampleY[i] = ot.Point(1, sample[i, 0])
        p = ot.Point(dim - 1)
        for j in range(dim - 1):
            p[j] = sample[i, j + 1]
        sampleX[i] = p

    sampleZ = ot.Sample(size, 1)
    for i in range(size):
        sampleZ[i] = ot.Point(1, sampleY[i, 0] * sampleY[i, 0])
    print("LinearModelFisher=",
          ot.LinearModelTest.LinearModelFisher(sampleY, sampleZ))
    print("LinearModelResidualMean=",
          ot.LinearModelTest.LinearModelResidualMean(sampleY, sampleZ))

    # Durbin Watson
    ot.RandomGenerator.SetSeed(5415)
    eps = ot.Normal(0, 20)
    f = ot.SymbolicFunction('x', '5+2*x+x^2-0.1*x^3')
    N = 15
    x = ot.Sample([[0], [1.42857], [2.85714], [4.28571], [5.71429], [7.14286],
                   [8.57143], [10], [11.4286], [12.8571], [14.2857], [15.7143],
                   [17.1429], [18.5714], [20]])
    y = f(x) + eps.getSample(N)
    linmodel = ot.LinearModelAlgorithm(x, y).getResult().getTrendCoefficients()
    dwTest = ot.LinearModelTest.LinearModelDurbinWatson(x, y)
    print('Durbin Watson = ', dwTest)

except:
    import sys
    print("t_LinearModelTest_std.py", sys.exc_info()[0], sys.exc_info()[1])

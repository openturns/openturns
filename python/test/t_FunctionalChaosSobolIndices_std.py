#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
from math import *

ot.TESTPREAMBLE()


# Problem parameters
dimension = 3
a = 7.0
b = 0.1
# Reference analytical values
meanTh = a / 2
covTh = (b ** 2 * pi ** 8) / 18.0 + \
    (b * pi ** 4) / 5.0 + (a ** 2) / 8.0 + 1.0 / 2.0
sob_1 = [(b * pi ** 4 / 5.0 + b ** 2 * pi ** 8 / 50.0 + 1.0 / 2.0)
         / covTh, (a ** 2 / 8.0) / covTh, 0.0]
sob_2 = [
    0.0, (b ** 2 * pi ** 8 / 18.0 - b ** 2 * pi ** 8 / 50.0) / covTh, 0.0]
sob_3 = [0.0]
sob_T1 = [sob_1[0] + sob_2[0] + sob_2[1] + sob_3[0], sob_1[1] + sob_2[0]
          + sob_2[2] + sob_3[0], sob_1[2] + sob_2[1] + sob_2[2] + sob_3[0]]
sob_T2 = [sob_2[0] + sob_2[1] + sob_3[0], sob_2[0]
          + sob_2[2] + sob_3[0], sob_2[1] + sob_2[2] + sob_3[0]]
sob_T3 = [sob_3[0]]

model = ot.SymbolicFunction(['xi1', 'xi2', 'xi3'], [
                            'sin(xi1) + (' + str(a) + ') * (sin(xi2)) ^ 2 + (' + str(b) + ') * xi3^4 * sin(xi1)'])

# Create the input distribution
distribution = ot.ComposedDistribution([ot.Uniform(-pi, pi)] * dimension)

size = 1000
x = distribution.getSample(size)
y = model(x)

# To reduce the time needed by the test
ot.ResourceMap.SetAsUnsignedInteger(
    "FittingTest-KolmogorovTestSamplingSize", 4)
algo = ot.FunctionalChaosAlgorithm(x, y)
algo.run()
result = algo.getResult()
sensitivity = ot.FunctionalChaosSobolIndices(result)

print(sensitivity.summary())

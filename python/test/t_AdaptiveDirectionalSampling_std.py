#!/usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()

# create a function
dim = 4
function = ot.SymbolicFunction(
    ['E', 'F', 'L', 'I'], ['F*L^3/(3.*E*I)'])

# create a distribution
distribution = ot.Normal(
    [50., 1.0, 10.0, 5.0], [1.0] * dim,
    ot.IdentityMatrix(dim))
vect = ot.RandomVector(distribution)
composite = ot.CompositeRandomVector(function, vect)
event = ot.ThresholdEvent(composite, ot.Less(), -3.0)

# create an ADS algorithm
n = int(1e4)
algo = ot.AdaptiveDirectionalSampling(event)
algo.setMaximumOuterSampling(n)
algo.setGamma([0.6, 0.4])

algo.run()
result = algo.getResult()
print(result)

# ADS-2+
algo2 = algo
algo2.setPartialStratification(True)
algo2.run()
print('T=', algo2.getTStatistic())
result = algo2.getResult()
print(result)

# DPADS-2
algo3 = algo2
algo3.setQuadrantOrientation([1.] * dim)  # enables DPADS-2, sets design
algo3.run()
result = algo3.getResult()
print(result)

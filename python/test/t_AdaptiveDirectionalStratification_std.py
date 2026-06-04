#!/usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# create a function
dim = 4
function = ot.SymbolicFunction(["E", "F", "L", "I"], ["F*L^3/(3.*E*I)"])

# create a distribution
distribution = ot.Normal([50.0, 1.0, 10.0, 5.0], [1.0] * dim, ot.IdentityMatrix(dim))
vect = ot.RandomVector(distribution)
composite = ot.CompositeRandomVector(function, vect)
event = ot.ThresholdEvent(composite, ot.Less(), -3.0)

# create an ADS algorithm
n = int(1e4)
algo = ot.AdaptiveDirectionalStratification(event)
algo.setMaximumOuterSampling(n)
algo.setGamma([0.6, 0.4])

algo.run()
result = algo.getResult()
print(result)

# ADS-2+
algo2 = algo
algo2.setPartialStratification(True)
algo2.run()
print("T=", algo2.getTStatistic())
result = algo2.getResult()
print(result)

# DPADS-2
algo3 = algo2
algo3.setQuadrantOrientation([1.0] * dim)  # enables DPADS-2, sets design
algo3.run()
result = algo3.getResult()
print(result)


# Test setter of event
ot.RandomGenerator.SetSeed(0)
X = ot.RandomVector(ot.Normal(2))
Y = ot.CompositeRandomVector(ot.SymbolicFunction(['X1','X2'], ['X1']),X)
event = ot.ThresholdEvent(Y, ot.Less(), -2.0)
algo = ot.AdaptiveDirectionalStratification(event)
algo.setMaximumOuterSampling(100000)
algo.setMaximumCoefficientOfVariation(0.01)
algo.run()
result = algo.getResult()
assert_almost_equal(result.getProbabilityEstimate(), ot.Normal().computeCDF(-2), 1.0e-1, 0.0)
ot.RandomGenerator.SetSeed(0)
Y2 = ot.CompositeRandomVector(ot.SymbolicFunction(['X1','X2'], ['2 * X1']),X)
event2 = ot.ThresholdEvent(Y2, ot.Less(), -2.0)
algo.setEvent(event2)
algo.run()
result2 = algo.getResult()
assert_almost_equal(result2.getProbabilityEstimate(), ot.Normal().computeCDF(-1), 1.0e-1, 0.0)

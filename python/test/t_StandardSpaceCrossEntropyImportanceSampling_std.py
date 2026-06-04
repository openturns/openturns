#!/usr/bin/env python

import openturns as ot
from openturns.testing import assert_almost_equal

ot.RandomGenerator.SetSeed(0)
distribution_R = ot.LogNormalMuSigma(300.0, 30.0, 0.0).getDistribution()
distribution_F = ot.Normal(75e3, 5e3)
marginals = [distribution_R, distribution_F]
distribution = ot.JointDistribution(marginals)


model = ot.SymbolicFunction(["R", "F"], ["R - F / (pi_ * 100.0)"])

vect = ot.RandomVector(distribution)
g = ot.CompositeRandomVector(model, vect)
event = ot.ThresholdEvent(g, ot.Less(), -50.0)
algo = ot.StandardSpaceCrossEntropyImportanceSampling(event, 0.3)

algo.run()
result = algo.getResult()

assert_almost_equal(result.getProbabilityEstimate(), 0.000126895, 1.0e-2, 0.0)

# from intersection
paraboloid = ot.SymbolicFunction(
    ["u1", "u2", "u3", "u4", "u5"], ["- u5 + u1^2 + u2^2 + u3^2 + u4^2"]
)
b = 3.5
db = 0.02
dim = 5
U = ot.Normal(dim)
X = ot.RandomVector(U)
Y = ot.CompositeRandomVector(paraboloid, X)
E1 = ot.ThresholdEvent(Y, ot.Less(), -b)
E3 = ot.ThresholdEvent(Y, ot.Greater(), -(b + db))
E4 = ot.IntersectionEvent([E1, E3])
algo = ot.StandardSpaceCrossEntropyImportanceSampling(E4, 0.35)


# Test setter of event
ot.RandomGenerator.SetSeed(0)
X = ot.RandomVector(ot.Normal())
Y = ot.CompositeRandomVector(ot.SymbolicFunction(["X"], ["X"]), X)
event = ot.ThresholdEvent(Y, ot.Less(), -2.0)
algo = ot.StandardSpaceCrossEntropyImportanceSampling(event, 0.25)
algo.setMaximumOuterSampling(100000)
algo.run()
result = algo.getResult()
assert_almost_equal(
    result.getProbabilityEstimate(), ot.Normal().computeCDF(-2), 1.0e-2, 0.0
)
ot.RandomGenerator.SetSeed(0)
Y2 = ot.CompositeRandomVector(ot.SymbolicFunction(["X"], ["2 * X"]), X)
event2 = ot.ThresholdEvent(Y2, ot.Less(), -2.0)
algo.setEvent(event2)
algo.run()
result2 = algo.getResult()
assert_almost_equal(
    result2.getProbabilityEstimate(), ot.Normal().computeCDF(-1), 1.0e-2, 0.0
)

#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

dim = 2
distribution = ot.Normal(dim)


X = ot.RandomVector(distribution)


# 1. Composite/Composite
f1 = ot.SymbolicFunction(['x'+str(i) for i in range(dim)], ['x0'])
f2 = ot.SymbolicFunction(['x'+str(i) for i in range(dim)], ['x1'])

Y1 = ot.CompositeRandomVector(f1, X)
Y2 = ot.CompositeRandomVector(f2, X)

e1 = ot.Event(Y1, ot.Less(), 0.0)
e2 = ot.Event(Y2, ot.Greater(), 0.0)

e3 = e1.intersect(e2)
#print('e3=', e3)

# sampling test
algo = ot.ProbabilitySimulationAlgorithm(e3)
algo.setMaximumOuterSampling(2500)
algo.setBlockSize(4)
algo.setMaximumCoefficientOfVariation(-0.1)
algo.run()
proba_e3 = algo.getResult().getProbabilityEstimate()
print("proba_e3 = %.3g" % proba_e3)
ott.assert_almost_equal(proba_e3, 0.25, 1e-2, 1e-2)

e4 = e1.join(e2)
#print('e4=', e4)

# sampling test
algo = ot.ProbabilitySimulationAlgorithm(e4)
algo.setMaximumOuterSampling(2500)
algo.setBlockSize(4)
algo.setMaximumCoefficientOfVariation(-0.1)
algo.run()
proba_e4 = algo.getResult().getProbabilityEstimate()
print("proba_e4 = %.3g" % proba_e4)
ott.assert_almost_equal(proba_e4, 0.75, 1e-2, 1e-2)

e5 = ot.IntersectionEvent([e1, e2])
#print('e5=', e5)

algo = ot.ProbabilitySimulationAlgorithm(e5)
algo.setMaximumOuterSampling(2500)
algo.setBlockSize(4)
algo.setMaximumCoefficientOfVariation(-0.1)
algo.run()
proba_e5 = algo.getResult().getProbabilityEstimate()
print("proba_e5 = %.3g" % proba_e5)
ott.assert_almost_equal(proba_e5, 0.25, 1e-2, 1e-2)


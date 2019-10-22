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

e1 = ot.ThresholdEvent(Y1, ot.Less(), 0.0)
e2 = ot.ThresholdEvent(Y2, ot.Greater(), 0.0)

e3 = e1.intersect(e2)
#print('e3=', e3)

proba_e3 = e3.getSample(10000).computeMean()[0]
print("proba_e3 = %.3g" % proba_e3)
ott.assert_almost_equal(proba_e3, 0.25, 1e-2, 1e-2)

e4 = e1.join(e2)
#print('e4=', e4)

proba_e4 = e4.getSample(10000).computeMean()[0]
print("proba_e4 = %.3g" % proba_e4)
ott.assert_almost_equal(proba_e4, 0.75, 1e-2, 1e-2)

e5 = ot.IntersectionEvent([e1, e2])
#print('e5=', e5)

proba_e5 = e5.getSample(10000).computeMean()[0]
print("proba_e5 = %.3g" % proba_e5)
ott.assert_almost_equal(proba_e5, 0.25, 1e-2, 1e-2)

e6 = ot.UnionEvent([e1, e2])
#print('e6=', e6)

proba_e6 = e6.getSample(10000).computeMean()[0]
print("proba_e6 = %.3g" % proba_e6)
ott.assert_almost_equal(proba_e6, 0.75, 1e-2, 1e-2)

# intersection of intersection
e7 = ot.IntersectionEvent([e1, ot.IntersectionEvent([e1, e2])])
ott.assert_almost_equal(e7.getSample(10000).computeMean()[0], 0.25, 1e-2, 1e-2)

e8 = ot.IntersectionEvent([ot.IntersectionEvent([e1, e2]), e1])
ott.assert_almost_equal(e8.getSample(10000).computeMean()[0], 0.25, 1e-2, 1e-2)

# union of union
e9 = ot.UnionEvent([e1, ot.UnionEvent([e1, e2])])
ott.assert_almost_equal(e9.getSample(10000).computeMean()[0], 0.75, 1e-2, 1e-2)

e10 = ot.UnionEvent([ot.UnionEvent([e1, e2]), e1])
ott.assert_almost_equal(e10.getSample(
    10000).computeMean()[0], 0.75, 1e-2, 1e-2)

# intersection of union
e11 = ot.IntersectionEvent([ot.UnionEvent([e1, e2]), ot.UnionEvent([e1, e2])])
ott.assert_almost_equal(e11.getSample(
    10000).computeMean()[0], 0.75, 1e-2, 1e-2)

# union of intersection
e12 = ot.UnionEvent([ot.IntersectionEvent([e1, e2]),
                     ot.IntersectionEvent([e1, e2])])
ott.assert_almost_equal(e12.getSample(
    10000).computeMean()[0], 0.25, 1e-2, 1e-2)

# through simulation


def sim_event(ev):
    experiment = ot.MonteCarloExperiment()
    algo = ot.ProbabilitySimulationAlgorithm(ev, experiment)
    algo.setMaximumOuterSampling(2500)
    algo.setBlockSize(4)
    algo.setMaximumCoefficientOfVariation(-1.0)
    algo.run()
    result = algo.getResult()
    return result.getProbabilityEstimate()


ott.assert_almost_equal(sim_event(e5), 0.25, 1e-2, 1e-2)

ott.assert_almost_equal(sim_event(e6), 0.75, 1e-2, 1e-2)

ott.assert_almost_equal(sim_event(e7), 0.25, 1e-2, 1e-2)


def subset_event(ev):
    algo = ot.SubsetSampling(ev)
    algo.setMaximumOuterSampling(2500)
    algo.setBlockSize(4)
    algo.run()
    result = algo.getResult()
    return result.getProbabilityEstimate()


ott.assert_almost_equal(subset_event(e3), 0.25, 1e-2, 1e-2)
ott.assert_almost_equal(subset_event(e5), 0.25, 1e-2, 1e-2)
ott.assert_almost_equal(subset_event(e6), 0.75, 1e-2, 1e-2)
ott.assert_almost_equal(subset_event(e7), 0.25, 1e-2, 1e-2)

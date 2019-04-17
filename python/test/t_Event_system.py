#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

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
algo.setMaximumOuterSampling(250)
algo.setBlockSize(4)
algo.setMaximumCoefficientOfVariation(-0.1)
algo.run()
print("proba_e3 = %.3g" % algo.getResult().getProbabilityEstimate())


e4 = e1.join(e2)
#print('e4=', e4)

# sampling test
algo = ot.ProbabilitySimulationAlgorithm(e4)
algo.setMaximumOuterSampling(250)
algo.setBlockSize(4)
algo.setMaximumCoefficientOfVariation(-0.1)
algo.run()
print("proba_e4 = %.3g" % algo.getResult().getProbabilityEstimate())


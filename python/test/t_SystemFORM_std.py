#!/usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()
# ot.Log.Show(ot.Log.ALL)
dim = 7

# root cause
mean = [200.0] * dim
mean[-1] = 60
mean[-2] = 60
sigma = [30.0] * dim
sigma[-1] = 15.0
R = ot.CorrelationMatrix(dim)
for i in range(5):
    for j in range(i):
        R[i, j] = 0.5
dist = ot.Normal(mean, sigma, R)

# leaf events
X = ot.RandomVector(dist)
inputs = ['M1', 'M2', 'M3', 'M4', 'M5', 'H', 'V']
e0 = ot.ThresholdEvent(ot.CompositeRandomVector(
    ot.SymbolicFunction(inputs, ['M1+M2+M4+M5-5*H']), X), ot.Less(), 0.0)
e1 = ot.ThresholdEvent(ot.CompositeRandomVector(
    ot.SymbolicFunction(inputs, ['M2+2*M3+M4-5*V']), X), ot.Less(), 0.0)
e2 = ot.ThresholdEvent(ot.CompositeRandomVector(ot.SymbolicFunction(
    inputs, ['M1+2*M3+2*M4+M5-5*H-5*V']), X), ot.Less(), 0.0)
e3 = ot.ThresholdEvent(ot.CompositeRandomVector(ot.SymbolicFunction(
    inputs, ['-(M1+M2+M4+M5-5*80.0)']), X), ot.Less(), 0.0)
e4 = ot.ThresholdEvent(ot.CompositeRandomVector(
    ot.SymbolicFunction(inputs, ['-(M2+2*M3+M4-5*70.0)']), X), ot.Less(), 0.0)
e5 = ot.ThresholdEvent(ot.CompositeRandomVector(ot.SymbolicFunction(
    inputs, ['-(M1+2*M3+2*M4+M5-5*80.0-5*70.0)']), X), ot.Less(), 0.0)

# solver
solver = ot.AbdoRackwitz()
solver.setMaximumIterationNumber(1000)
solver.setMaximumAbsoluteError(1.0e-3)
solver.setMaximumRelativeError(1.0e-3)
solver.setMaximumResidualError(1.0e-3)
solver.setMaximumConstraintError(1.0e-3)

for event in [e0, e3, e4, e5, e1, e2]:
    algo = ot.FORM(solver, event, mean)
    algo.run()
    result = algo.getResult()
    print('beta = %.6g' % result.getGeneralisedReliabilityIndex())

# system event in DNF form (union of intersections)
event = ot.UnionEvent([ot.IntersectionEvent([e0, e3, e4, e5]), ot.IntersectionEvent(
    [e1, e3, e4, e5]), ot.IntersectionEvent([e2, e3, e4, e5])])

# sampling test
pf_sim = event.getSample(100000).computeMean()[0]
print('pf_sim = %.6g' % pf_sim)
ott.assert_almost_equal(pf_sim, 0.00384, 1e-4, 1e-4)

# system FORM
algo = ot.SystemFORM(solver, event, mean)
algo.run()
result = algo.getResult()
pf_sysform = result.getEventProbability()
print('pf_sysform = %.6g' % pf_sysform)
ott.assert_almost_equal(pf_sysform, 0.00418394, 1e-4, 1e-4)

for form_result in result.getFORMResultCollection():
    print(' beta=', form_result.getGeneralisedReliabilityIndex())

#!/usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# We create a numerical math function
dim = 2

# event
f = ot.SymbolicFunction(['x0', 'x1'], ['5.0-x1-0.5*(x0-0.1)^2'])
dist = ot.Normal(dim)
X = ot.RandomVector(dist)
Y = ot.CompositeRandomVector(f, X)
event = ot.ThresholdEvent(Y, ot.Less(), 0.0)

# solver
solver = ot.Cobyla()
solver.setMaximumIterationNumber(100)
solver.setMaximumAbsoluteError(1.0e-10)
solver.setMaximumRelativeError(1.0e-10)
solver.setMaximumResidualError(1.0e-10)
solver.setMaximumConstraintError(1.0e-10)

# algorithm
start_pt = [0.0] * dim
algo = ot.MultiFORM(solver, event, start_pt)
algo.setMaximumNumberOfDesignPoints(4)
algo.run()
result = algo.getResult()

print("result=", result)

pf_multi = result.getEventProbability()
ott.assert_almost_equal(pf_multi, 0.00267684, 1e-4, 1e-4)
print("event probability=%.6f" % pf_multi)

n_design_pts = len(result.getFORMResultCollection())
print('n design points:', n_design_pts)
assert n_design_pts == 2, "should get 2 design points"

for form_result in result.getFORMResultCollection():
    print('-'*50)
    print("generalized reliability indexes=",
          form_result.getGeneralisedReliabilityIndex())
    print("pf=", form_result.getEventProbability())
    print("standard space design points=",
          form_result.getStandardSpaceDesignPoint())
    print("physical space design points=",
          form_result.getPhysicalSpaceDesignPoint())
    print("is standard point origin in failure space? %s" % (
        form_result.getIsStandardPointOriginInFailureSpace() and "true" or "false"))
    print("importance factors=", form_result.getImportanceFactors())
    print("Hasofer reliability indexes=",
          form_result.getHasoferReliabilityIndex())

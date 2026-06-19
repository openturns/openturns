#!/usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# We create a numerical math function
dim = 2

# event
f = ot.SymbolicFunction(["x0", "x1"], ["5.0-x1-0.5*(x0-0.1)^2"])
dist = ot.Normal(dim)
X = ot.RandomVector(dist)
Y = ot.CompositeRandomVector(f, X)
event = ot.ThresholdEvent(Y, ot.Less(), 0.0)

# solver
solver = ot.Cobyla()
solver.setStartingPoint([0.0] * dim)
solver.setMaximumCallsNumber(1000)
solver.setMaximumAbsoluteError(1.0e-10)
solver.setMaximumRelativeError(1.0e-10)
solver.setMaximumResidualError(1.0e-10)
solver.setMaximumConstraintError(1.0e-10)

# algorithm
algo = ot.MultiFORM(solver, event)
algo.setMaximumDesignPointsNumber(4)
algo.run()
result = algo.getResult()

print("result=", result)

pf_multi = result.getEventProbability()
ott.assert_almost_equal(pf_multi, 0.00281872, 1e-4, 1e-4)
print("event probability=%.6f" % pf_multi)

n_design_pts = len(result.getFORMResultCollection())
print("n design points:", n_design_pts)
assert n_design_pts == 2, "should get 2 design points"

for form_result in result.getFORMResultCollection():
    print("-" * 50)
    print(
        "generalized reliability indexes=", form_result.getGeneralisedReliabilityIndex()
    )
    print("pf=", form_result.getEventProbability())
    print("standard space design points=", form_result.getStandardSpaceDesignPoint())
    print("physical space design points=", form_result.getPhysicalSpaceDesignPoint())
    print(
        "is standard point origin in failure space? %s"
        % (form_result.getIsStandardPointOriginInFailureSpace() and "true" or "false")
    )
    print("importance factors=", form_result.getImportanceFactors())
    print("Hasofer reliability indexes=", form_result.getHasoferReliabilityIndex())

# Test with two orthogonal circles
# Failure domain: union of (x+5)^2+y^2 < 1 and x^2+(y+5)^2 < 1
# Design points at (-4,0) and (0,-4), both beta=4, orthogonal cosines
f2 = ot.SymbolicFunction(["x", "y"], ["min((x+5)^2+y^2-1, x^2+(y+5)^2-1)"])
dist2 = ot.Normal(2)
X2 = ot.RandomVector(dist2)
Y2 = ot.CompositeRandomVector(f2, X2)
event2 = ot.ThresholdEvent(Y2, ot.Less(), 0.0)

solver2 = ot.Cobyla()
solver2.setStartingPoint([0.0] * 2)
solver2.setMaximumCallsNumber(2000)
solver2.setMaximumAbsoluteError(1.0e-6)
solver2.setMaximumRelativeError(1.0e-6)
solver2.setMaximumResidualError(1.0e-6)
solver2.setMaximumConstraintError(1.0e-6)

algo2 = ot.MultiFORM(solver2, event2)
algo2.setMaximumDesignPointsNumber(4)
algo2.run()
result2 = algo2.getResult()

print("result2=", result2)

pf_multi2 = result2.getEventProbability()
n_design_pts2 = len(result2.getFORMResultCollection())
print("n design points:", n_design_pts2)
ott.assert_almost_equal(pf_multi2, 6.334e-05, 1e-6, 1e-6)
assert n_design_pts2 == 2, "should get 2 design points"

for form_result in result2.getFORMResultCollection():
    print("-" * 50)
    beta = form_result.getHasoferReliabilityIndex()
    ott.assert_almost_equal(beta, 4.0, 1e-2, 1e-2)
    print("standard space design points=", form_result.getStandardSpaceDesignPoint())
    print("Hasofer reliability indexes=", beta)

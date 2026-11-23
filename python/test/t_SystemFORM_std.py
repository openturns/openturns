#!/usr/bin/env python

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
inputs = ["M1", "M2", "M3", "M4", "M5", "H", "V"]
e0 = ot.ThresholdEvent(
    ot.CompositeRandomVector(ot.SymbolicFunction(inputs, ["M1+M2+M4+M5-5*H"]), X),
    ot.Less(),
    0.0,
)
e1 = ot.ThresholdEvent(
    ot.CompositeRandomVector(ot.SymbolicFunction(inputs, ["M2+2*M3+M4-5*V"]), X),
    ot.Less(),
    0.0,
)
e2 = ot.ThresholdEvent(
    ot.CompositeRandomVector(
        ot.SymbolicFunction(inputs, ["M1+2*M3+2*M4+M5-5*H-5*V"]), X
    ),
    ot.Less(),
    0.0,
)
e3 = ot.ThresholdEvent(
    ot.CompositeRandomVector(ot.SymbolicFunction(inputs, ["-(M1+M2+M4+M5-5*80.0)"]), X),
    ot.Less(),
    0.0,
)
e4 = ot.ThresholdEvent(
    ot.CompositeRandomVector(ot.SymbolicFunction(inputs, ["-(M2+2*M3+M4-5*70.0)"]), X),
    ot.Less(),
    0.0,
)
e5 = ot.ThresholdEvent(
    ot.CompositeRandomVector(
        ot.SymbolicFunction(inputs, ["-(M1+2*M3+2*M4+M5-5*80.0-5*70.0)"]), X
    ),
    ot.Less(),
    0.0,
)

# solver
solver = ot.AbdoRackwitz()
solver.setStartingPoint(mean)
solver.setMaximumIterationNumber(1000)
solver.setMaximumAbsoluteError(1.0e-3)
solver.setMaximumRelativeError(1.0e-3)
solver.setMaximumResidualError(1.0e-3)
solver.setMaximumConstraintError(1.0e-3)

for event in [e0, e3, e4, e5, e1, e2]:
    algo = ot.FORM(solver, event)
    algo.run()
    result = algo.getResult()
    print("beta = %.6g" % result.getGeneralisedReliabilityIndex())

# system event in DNF form (union of intersections)
event = ot.UnionEvent(
    [
        ot.IntersectionEvent([e0, e3, e4, e5]),
        ot.IntersectionEvent([e1, e3, e4, e5]),
        # ot.IntersectionEvent([e2, e3, e4, e5]),
    ]
)

# sampling test
pf_sim = event.getSample(10000).computeMean()[0]
print("pf_sim = %.6g" % pf_sim)
ott.assert_almost_equal(pf_sim, 0.0023, 1e-3, 1e-3)

# system FORM
algo = ot.SystemFORM(solver, event)
algo.run()
result = algo.getResult()
pf_sysform = result.getEventProbability()
print("pf_sysform = %.6g" % pf_sysform)
ott.assert_almost_equal(pf_sysform, 0.00234983, 1e-4, 1e-4)

for form_result in result.getFORMResultCollection():
    print(" beta=", form_result.getGeneralisedReliabilityIndex())

# gh #3444: system event reusing the same limit state twice, with a
# generalised reliability index < 0 (origin in the failure domain)
X3444 = ot.RandomVector(ot.Normal(3))


def event3444(formula, operator, threshold):
    function = ot.SymbolicFunction(["u1", "u2", "u3"], [formula])
    return ot.ThresholdEvent(
        ot.CompositeRandomVector(function, X3444), operator, threshold
    )


h_gt = event3444("u1+u2", ot.Greater(), 0.5)
h_lt = event3444("u1+u2", ot.Less(), 0.5)
g1 = event3444("u1-u2+u3", ot.Greater(), 0.4)
g2 = event3444("u2+u3", ot.Greater(), 0.3)
solver3444 = ot.Cobyla()
solver3444.setStartingPoint([0.0] * 3)
for name, system_event, exact in [
    (
        "opposite sides",
        ot.UnionEvent(
            [ot.IntersectionEvent([h_gt, g1]), ot.IntersectionEvent([h_lt, g2])]
        ),
        0.335774,
    ),
    (
        "same side",
        ot.UnionEvent(
            [ot.IntersectionEvent([h_lt, g1]), ot.IntersectionEvent([h_lt, g2])]
        ),
        0.371910,
    ),
]:
    algo3444 = ot.SystemFORM(solver3444, system_event)
    algo3444.run()
    pf3444 = algo3444.getResult().getEventProbability()
    print(name, "pf_sysform = %.6g" % pf3444)
    ott.assert_almost_equal(pf3444, exact, 1e-4, 1e-4)

# zero thresholds exercise the gradient fallback (beta == 0 at the origin)
z1_lt = event3444("u1", ot.Less(), 0.0)
z1_gt = event3444("u1", ot.Greater(), 0.0)
z2_lt = event3444("u1+u2", ot.Less(), 0.0)
for name, system_event, exact in [
    ("zero thresholds 3/8", ot.IntersectionEvent([z1_lt, z2_lt]), 0.375),
    ("zero thresholds 1/8", ot.IntersectionEvent([z1_gt, z2_lt]), 0.125),
]:
    algo3444 = ot.SystemFORM(solver3444, system_event)
    algo3444.run()
    pf3444 = algo3444.getResult().getEventProbability()
    print(name, "pf_sysform = %.6g" % pf3444)
    ott.assert_almost_equal(pf3444, exact, 1e-4, 1e-4)

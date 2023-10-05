#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# need a proper LS solver
if not ot.PlatformInfo.HasFeature("cminpack") or not ot.PlatformInfo.HasFeature("ceres"):
    exit(0)

size = 10000

# gumbel
distribution = ot.Gumbel(1.5, -0.5)
print("distribution=", distribution)
sample = distribution.getSample(size)
factory = ot.QuantileMatchingFactory(ot.Gumbel(), [0.01, 0.99])
inf_dist = factory.build(sample)
print("estimated distribution=", inf_dist)
ott.assert_almost_equal(
    distribution.getParameter(), inf_dist.getParameter(), 1e-2, 1e-2
)

# set (a,b) out of (r, t, a, b)
distribution = ot.Beta(2.3, 2.2, -1.0, 1.0)
p_ref = distribution.getParameter()
print("distribution=", distribution)
sample = distribution.getSample(size)
factory = ot.QuantileMatchingFactory(ot.Beta(), [0.01, 0.99])
factory.setKnownParameter([-1.0, 1.0], [2, 3])
inf_dist = factory.build(sample)
print("estimated distribution=", inf_dist)
ott.assert_almost_equal(inf_dist.getParameter(), p_ref, 1e-2, 1e-2)

# full probas
probabilities = [0.01, 1 / 3, 2 / 3, 0.99]
factory = ot.QuantileMatchingFactory(ot.Beta(), probabilities)
quantiles = [distribution.computeQuantile(pi)[0] for pi in probabilities]
inf_dist = factory.buildFromQuantiles(quantiles)
print("estimated distribution (quantiles)=", inf_dist)
ott.assert_almost_equal(inf_dist.getParameter(), p_ref, 1e-2, 1e-2)

# from quantiles, known parameters
factory = ot.QuantileMatchingFactory(ot.Beta(), [0.01, 0.99])
factory.setKnownParameter([-1.0, 1.0], [2, 3])
quantiles = [-0.847, 0.867]
inf_dist = factory.buildFromQuantiles(quantiles)
print("estimated distribution (quantiles)=", inf_dist)
probabilities = factory.getProbabilities()
ott.assert_almost_equal(inf_dist.getParameter(), p_ref, 1e-2, 1e-2)

# ill defined case
probabilities = [0.05, 0.25, 0.75, 0.95]
factory = ot.QuantileMatchingFactory(ot.Beta(), probabilities)
quantiles = [-4.0, 1.0, 2.0, 10.0]
ok = False
try:
    match_dist = factory.buildFromQuantiles(quantiles)
except Exception:
    ok = True
assert ok, "should raise"

# Normal, optimal far away
distribution = ot.Normal(17.0, 1.0)
print("distribution=", distribution)
sample = distribution.getSample(size)
factory = ot.QuantileMatchingFactory(ot.Normal(), [0.01, 0.99])
inf_dist = factory.build(sample)
ott.assert_almost_equal(inf_dist.getParameter(), distribution.getParameter(), 1e-2, 1e-2)

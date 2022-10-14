#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()
ot.PlatformInfo.SetNumericalPrecision(3)

size = 10000

distribution = ot.Gumbel(1.5, -0.5)
print("distribution=", distribution)
sample = distribution.getSample(size)
factory = ot.QuantileMatchingFactory(ot.Gumbel())
inf_distribution = factory.build(sample)
print("estimated distribution=", inf_distribution)
ott.assert_almost_equal(
    distribution.getParameter(), inf_distribution.getParameter(), 1e-2, 1e-2
)

# set (a,b) out of (r, t, a, b)
distribution = ot.Beta(2.3, 2.2, -1.0, 1.0)
print("distribution=", distribution)
sample = distribution.getSample(size)
factory = ot.QuantileMatchingFactory(ot.Beta())
factory.setKnownParameter([-1.0, 1.0], [2, 3])
inf_distribution = factory.build(sample)
print("estimated distribution=", inf_distribution)
ott.assert_almost_equal(
    distribution.getParameter(), inf_distribution.getParameter(), 1e-2, 1e-2
)

# from quantiles
probabilities = [0.01, 1 / 3, 2 / 3, 0.99]
factory = ot.QuantileMatchingFactory(ot.Beta(), probabilities)
quantiles = [distribution.computeQuantile(pi)[0] for pi in probabilities]
inf_distribution = factory.buildFromQuantiles(quantiles)
print("estimated distribution (quantiles)=", inf_distribution)
ott.assert_almost_equal(
    distribution.getParameter(), inf_distribution.getParameter(), 1e-2, 1e-2
)

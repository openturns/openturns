#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

size = 1000

distribution = ot.Pareto(3.3, 7.5, 0.0)
print("distribution=", distribution)
sample = distribution.getSample(size)
factory = ot.LeastSquaresDistributionFactory(ot.Pareto())
factory.setKnownParameter([0.0], [2])
inf_dist = factory.build(sample)
print("estimated distribution=", inf_dist)
ott.assert_almost_equal(
    inf_dist.getParameter(), distribution.getParameter(), 1e-2, 1e-2
)

# Normal, optimal far away
distribution = ot.Normal(17.0, 1.0)
print("distribution=", distribution)
sample = distribution.getSample(size)
factory = ot.QuantileMatchingFactory(ot.Normal(), [0.01, 0.99])
inf_dist = factory.build(sample)
print("estimated distribution=", inf_dist)
ott.assert_almost_equal(
    inf_dist.getParameter(), distribution.getParameter(), 1e-1, 1e-1
)

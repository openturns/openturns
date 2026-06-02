#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(0)

# Test cases: (base distribution, truncation interval)
coll = [(ot.Exponential(), ot.Interval(-1.0, 1.0)),
        (ot.Normal(), ot.Interval(-1.0, 1.0)),
        (ot.Triangular(-2.0, -0.5, 3.0), ot.Interval(-1.0, 1.0)),
        ]
size = 10000
for estimateParameters in [False, True]:
    for distribution, interval in coll:
        print("distribution=", distribution)
        sample = ot.TruncatedDistribution(distribution, interval).getSample(size)
        factory = otexp.TruncatedDistributionFactory(distribution)
        factory.setEstimateParameters(estimateParameters)
        dist_est = factory.build(sample).getImplementation()
        print("estimated distribution=", dist_est)
        assert "TruncatedDistribution" in str(dist_est)
        for i in range(distribution.getDimension()):
            if dist_est.getBounds().getFiniteLowerBound()[i]:
                ott.assert_almost_equal(dist_est.getBounds().getLowerBound()[i], interval.getLowerBound()[i], 0.0, 1e-3)
            if dist_est.getBounds().getFiniteUpperBound()[i]:
                ott.assert_almost_equal(dist_est.getBounds().getUpperBound()[i], interval.getUpperBound()[i], 0.0, 1e-3)

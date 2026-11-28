#! /usr/bin/env python

import openturns as ot

import openturns.testing as ott

ot.TESTPREAMBLE()


distribution = ot.LogNormal(1.5, 2.5, -1.5)
size = 10000
sample = distribution.getSample(size)
factory = ot.LogNormalFactory()
# Default estimator is the local likelihood maximizer
estimatedDistribution = factory.build(sample)
ott.assert_almost_equal(
    estimatedDistribution.getParameter(),
    [1.4653145870837043, 2.503644452403846, -1.4997189373077306],
)
estimatedDistribution = factory.build(sample, 0)
ott.assert_almost_equal(
    estimatedDistribution.getParameter(),
    [1.4653145870837043, 2.503644452403846, -1.4997189373077306],
)
estimatedDistribution = factory.build(sample, 1)
ott.assert_almost_equal(
    estimatedDistribution.getParameter(),
    [2.4277587012992106, 2.0424293078874363, -1.5037171697567402],
)
estimatedDistribution = factory.build(sample, 2)
ott.assert_almost_equal(
    estimatedDistribution.getParameter(),
    [4.641674507180531, 1.4215894511254732, -195.15997062681078],
)
ott.assert_almost_equal(
    factory.build(sample, 3).getParameter(),
    [1.4660953812178277, 2.502031386410347, -1.499869565025487],
)
estimatedDistribution = factory.build()
ott.assert_almost_equal(estimatedDistribution.getParameter(), [0.0, 1.0, 0.0])
estimatedDistribution = factory.build(distribution.getParameter())
ott.assert_almost_equal(estimatedDistribution.getParameter(), [1.5, 2.5, -1.5])
estimatedLogNormal = factory.buildAsLogNormal(sample)
ott.assert_almost_equal(distribution.getParameter(), [1.5, 2.5, -1.5])
ott.assert_almost_equal(
    estimatedLogNormal.getParameter(),
    [1.4653145870837043, 2.503644452403846, -1.4997189373077306],
)
estimatedLogNormal = factory.buildAsLogNormal()
ott.assert_almost_equal(estimatedLogNormal.getParameter(), [0.0, 1.0, 0.0])
estimatedLogNormal = factory.buildAsLogNormal(distribution.getParameter())
ott.assert_almost_equal(estimatedLogNormal.getParameter(), [1.5, 2.5, -1.5])

# --- LogNormalFactory: invalid shifts are rejected ---
with ott.assert_raises(Exception):
    ot.LogNormalFactory().buildMethodOfLeastSquares(
        ot.Sample([[1.0], [2.0], [3.0]]), 5.0
    )
with ott.assert_raises(Exception):
    ot.ParetoFactory().buildMethodOfLeastSquares(ot.Sample([[1.0], [2.0], [3.0]]), 5.0)

# --- LogNormalFactory: local likelihood with no interior stationary point ---
# The profile log-likelihood increases towards the sample minimum, so the
# local likelihood estimator is undefined and the modified moments estimator
# is used instead (a spurious far-left root is discarded)
degenerateSample = ot.Sample([[1.5], [3.0], [10.0], [15.0], [23.0], [59.0]])
estimatedDegenerate = ot.LogNormalFactory().buildAsLogNormal(degenerateSample, 0)
ott.assert_almost_equal(
    estimatedDegenerate.getParameter(), [2.872298307618225, 0.7688215878291091, -5.172820948624551]
)
# The default estimator takes the same path
ott.assert_almost_equal(
    ot.LogNormalFactory().buildAsLogNormal(degenerateSample).getParameter(),
    [2.872298307618225, 0.7688215878291091, -5.172820948624551],
)

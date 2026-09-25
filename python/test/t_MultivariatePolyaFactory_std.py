#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

distribution = otexp.MultivariatePolya(5, [1, 1, 1], 1)
size = 20000
sample = distribution.getSample(size)
factory = otexp.MultivariatePolyaFactory()
estimatedDistribution = factory.build(sample)
assert estimatedDistribution.getDimension() == 3
estimatedDistribution = factory.build()
assert estimatedDistribution.getDimension() == 1
estimatedMultivariatePolya = factory.buildAsMultivariatePolya(sample)
assert estimatedMultivariatePolya.getDimension() == 3
defaultMultivariatePolya = factory.buildAsMultivariatePolya()
assert defaultMultivariatePolya.getDimension() == 1
# The estimated distribution must be a valid distribution
ott.assert_almost_equal(
    estimatedMultivariatePolya.computeCDF(
        [estimatedMultivariatePolya.getRange().getUpperBound()[i] + 0.5 for i in range(3)]
    ),
    1.0,
)
# The concentration parameters are recovered by the moment estimator
ott.assert_almost_equal(estimatedMultivariatePolya.getC(), 1.0, 0.2, 0.0)
ott.assert_almost_equal(estimatedMultivariatePolya.getAlpha(), [1, 1, 1], 0.2, 0.0)
# The description is inherited from the sample
sample.setDescription(["color A", "color B", "color C"])
estimatedDistribution2 = factory.build(sample)
desc = estimatedDistribution2.getDescription()
assert [desc[i] for i in range(desc.getSize())] == ["color A", "color B", "color C"]
# An underdispersed sample (pure multinomial) leads to the degenerate c = 0 member
multinomialSample = ot.Multinomial(5, [0.25, 0.5, 0.25]).getSample(20000)
degenerate = factory.buildAsMultivariatePolya(multinomialSample)
ott.assert_almost_equal(degenerate.getC(), 0.0)


with ott.assert_raises(TypeError):
    factory.buildAsMultivariatePolya(ot.Sample([[3, 1], [4, 2], [5, 0]]))
with ott.assert_raises(TypeError):
    factory.buildAsMultivariatePolya(ot.Sample([[1.5, 1.5], [2.0, 1.0]]))
with ott.assert_raises(TypeError):
    factory.buildAsMultivariatePolya(ot.Sample([[0, 0], [0, 0], [0, 0]]))
with ott.assert_raises(TypeError):
    factory.buildAsMultivariatePolya(ot.Sample([[3, 0], [float("nan"), 1.0]]))
with ott.assert_raises(TypeError):
    factory.buildAsMultivariatePolya(ot.Sample([[2, 1]]))
with ott.assert_raises(TypeError):
    factory.buildAsMultivariatePolya(ot.Sample([[3, -1], [2, 0]]))
with ott.assert_raises(TypeError):
    factory.buildAsMultivariatePolya(ot.Sample([[3, 0], [3, 0], [3, 0]]))
print("OK")

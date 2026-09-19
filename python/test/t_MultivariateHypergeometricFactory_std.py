#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

# Instantiate one distribution object
distribution = otexp.MultivariateHypergeometric(5, [3, 3, 3])
size = 10000
sample = distribution.getSample(size)
factory = otexp.MultivariateHypergeometricFactory()
estimatedDistribution = factory.build(sample)
estimatedDistribution = factory.build()
estimatedMultivariateHypergeometric = factory.buildAsMultivariateHypergeometric(sample)
defaultMultivariateHypergeometric = factory.buildAsMultivariateHypergeometric()
# The default distribution is the canonical 1D instance
assert defaultMultivariateHypergeometric.getDimension() == 1
# The estimated parameters recover the generative ones
ott.assert_almost_equal(estimatedMultivariateHypergeometric.getN(), 5)
ott.assert_almost_equal(estimatedMultivariateHypergeometric.getColors(), [3, 3, 3])
# The estimated distribution must be a valid distribution
ott.assert_almost_equal(
    estimatedMultivariateHypergeometric.computeCDF(
        [estimatedMultivariateHypergeometric.getRange().getUpperBound()[i] + 0.5 for i in range(3)]
    ),
    1.0,
)
# The description is inherited from the sample
sample.setDescription(["color A", "color B", "color C"])
estimatedDistribution2 = factory.build(sample)
desc = estimatedDistribution2.getDescription()
assert [desc[i] for i in range(desc.getSize())] == ["color A", "color B", "color C"]

# Error cases


with ott.assert_raises(TypeError):
    factory.buildAsMultivariateHypergeometric(ot.Sample([[3, 1], [4, 2], [5, 0]]))
with ott.assert_raises(TypeError):
    factory.buildAsMultivariateHypergeometric(ot.Sample([[1.5, 1.5], [2.0, 1.0]]))
with ott.assert_raises(TypeError):
    factory.buildAsMultivariateHypergeometric(ot.Sample([[0, 0], [0, 0], [0, 0]]))
with ott.assert_raises(TypeError):
    factory.buildAsMultivariateHypergeometric(ot.Sample([[3, 0], [float("nan"), 1.0]]))
with ott.assert_raises(TypeError):
    factory.buildAsMultivariateHypergeometric(ot.Sample([[2, 1]]))
with ott.assert_raises(TypeError):
    factory.buildAsMultivariateHypergeometric(ot.Sample([[3, -1], [2, 0]]))
print("OK")

#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

distribution = ot.Multinomial(5, [0.25] * 3)
size = 10000
sample = distribution.getSample(size)
factory = ot.MultinomialFactory()
estimatedDistribution = factory.build(sample)
assert estimatedDistribution.getDimension() == 3
estimatedDistribution = factory.build()
assert estimatedDistribution.getDimension() == 1
estimatedMultinomial = factory.buildAsMultinomial(sample)
assert estimatedMultinomial.getDimension() == 3
estimatedMultinomial = factory.buildAsMultinomial()
assert estimatedMultinomial.getDimension() == 1

# Deterministic case: N = 5, p = [0.45, 0.55]
known = ot.Sample([[5, 0], [0, 5], [3, 2], [1, 4]])
estimated = factory.buildAsMultinomial(known)
ott.assert_almost_equal(estimated.getN(), 5)
ott.assert_almost_equal(estimated.getP(), [0.45, 0.55])
# Description propagation
known.setDescription(["a", "b"])
assert list(factory.buildAsMultinomial(known).getDescription()) == ["a", "b"]
# Statistical consistency: every row sums to N
true_dist = ot.Multinomial(7, [0.3, 0.4, 0.3])
big_sample = true_dist.getSample(5000)
estimated2 = factory.buildAsMultinomial(big_sample)
ott.assert_almost_equal(estimated2.getN(), 7)
ott.assert_almost_equal(estimated2.getP(), [0.3, 0.4, 0.3], 0.0, 0.05)
# The empirical estimator formula in the defective case
defective = ot.Multinomial(8, [0.2, 0.3])
def_sample = defective.getSample(300)
est_def = factory.buildAsMultinomial(def_sample)
atoms = def_sample.computeMean()
totals = [float(sum(def_sample[i])) for i in range(def_sample.getSize())]
ott.assert_almost_equal(est_def.getN(), max(totals))
ott.assert_almost_equal(est_def.getP(), atoms / max(totals))
# Default usages
default_dist = factory.buildAsMultinomial()
ott.assert_almost_equal(default_dist.getN(), 1)
ott.assert_almost_equal(default_dist.getP(), [0.5])

# Error cases
with ott.assert_raises(TypeError):
    factory.build(ot.Sample([[1.0]]))
with ott.assert_raises(TypeError):
    factory.build(ot.Sample([[1, 2], [1.5, 3]]))
with ott.assert_raises(TypeError):
    factory.build(ot.Sample([[1, -2], [1, 3]]))
with ott.assert_raises(TypeError):
    factory.build(ot.Sample([[1, float("nan")], [1, 3]]))
with ott.assert_raises(TypeError):
    factory.build(ot.Sample([[1, float("inf")], [1, 3]]))
with ott.assert_raises(TypeError):
    factory.build(ot.Sample([[0, 0], [0, 0]]))
# A one-dimensional sample is a valid multinomial (binomial)
est_1d = factory.buildAsMultinomial(ot.Sample([[3], [4], [5], [2]]))
ott.assert_almost_equal(est_1d.getN(), 5)
ott.assert_almost_equal(est_1d.getP(), [0.7])

print("OK")

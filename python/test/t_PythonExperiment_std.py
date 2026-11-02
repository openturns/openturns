#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()


class WeightedDesign:
    """Duck-typed experiment with weighted generation, see issue #3171."""

    def generate(self):
        ot.RandomGenerator.SetSeed(0)
        return ot.Normal(2).getSample(4)

    def generateWithWeights(self):
        ot.RandomGenerator.SetSeed(0)
        return (ot.Normal(2).getSample(4), [0.1, 0.2, 0.3, 0.4])


# with generateWithWeights, custom weights come through
weighted = ot.Experiment(WeightedDesign())
weights2 = ot.Point()
sample2 = weighted.generateWithWeights(weights2)
assert sample2.getSize() == 4
ott.assert_almost_equal(weights2, [0.1, 0.2, 0.3, 0.4])

# mismatched weights are rejected


class BadDesign:
    def generate(self):
        return ot.Normal(2).getSample(4)

    def generateWithWeights(self):
        return (ot.Normal(2).getSample(4), [0.5, 0.5])


with ott.assert_raises(TypeError):
    bad = ot.Experiment(BadDesign())
    bad.generateWithWeights(ot.Point())

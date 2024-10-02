#!/usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
from openturns.testing import assert_almost_equal
from openturns.usecases import ishigami_function

ot.RandomGenerator.SetSeed(0)
im = ishigami_function.IshigamiModel()
input_names = im.inputDistribution.getDescription()
n = 250
sampleX = im.inputDistribution.getSample(n)
sampleY = im.model(sampleX)

ot.ResourceMap.SetAsUnsignedInteger("SobolIndicesAlgorithm-DefaultBootstrapSize", 100)

mySobol = otexp.RankSobolSensitivityAlgorithm(sampleX, sampleY)
indices = mySobol.getFirstOrderIndices()
assert_almost_equal(indices, [0.208654, 0.493591, -0.0669488], 1.0e-4, 1.0e-2)

interval = mySobol.getFirstOrderIndicesInterval()
assert_almost_equal(
    interval.getLowerBound(), [0.117529, 0.409689, -0.176039], 1.0e-4, 1.0e-2
)
assert_almost_equal(
    interval.getUpperBound(), [0.340675, 0.560271, 0.0857027], 1.0e-4, 1.0e-2
)

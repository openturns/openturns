#!/usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
from openturns.testing import assert_almost_equal
from openturns.usecases import ishigami_function

ot.RandomGenerator.SetSeed(0)
im = ishigami_function.IshigamiModel()
input_names = im.distributionX.getDescription()
n = 250
sampleX = im.distributionX.getSample(n)
sampleY = im.model(sampleX)

ot.ResourceMap.SetAsUnsignedInteger('SobolIndicesAlgorithm-DefaultBootstrapSize', 100)

mySobol = otexp.RankSobolSensitivityAlgorithm(sampleX,sampleY, n)
indices = mySobol.getFirstOrderIndices()

assert_almost_equal(indices[0], 0.208654, 1.0e-4, 1.0e-2)
assert_almost_equal(indices[1], 0.493591, 1.0e-4, 1.0e-2)
assert_almost_equal(indices[2], -0.0669488, 1.0e-4, 1.0e-2)

interval = mySobol.getFirstOrderIndicesInterval()

assert_almost_equal(interval.getLowerBound()[0], 0.117529, 1.0e-4, 1.0e-2)
assert_almost_equal(interval.getLowerBound()[1], 0.409689, 1.0e-4, 1.0e-2)
assert_almost_equal(interval.getLowerBound()[2], -0.176039, 1.0e-4, 1.0e-2)


assert_almost_equal(interval.getUpperBound()[0], 0.340675, 1.0e-4, 1.0e-2)
assert_almost_equal(interval.getUpperBound()[1], 0.560271, 1.0e-4, 1.0e-2)
assert_almost_equal(interval.getUpperBound()[2], 0.0857027, 1.0e-4, 1.0e-2)



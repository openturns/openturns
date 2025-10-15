#!/usr/bin/env python

import openturns as ot
from openturns.testing import assert_almost_equal
from openturns.usecases import ishigami_function

ot.RandomGenerator.SetSeed(0)
im = ishigami_function.IshigamiModel()
input_names = im.distribution.getDescription()
n = 250
sampleX = im.distribution.getSample(n)
sampleY = im.model(sampleX)

ot.ResourceMap.SetAsUnsignedInteger("SobolIndicesAlgorithm-DefaultBootstrapSize", 100)

ot.RandomGenerator.SetSeed(42)  # Set seed for bootstrap interval computation algorithm
mySobol = ot.RankSobolSensitivityAlgorithm(sampleX, sampleY)
indices = mySobol.getFirstOrderIndices()
assert_almost_equal(indices, [0.208654, 0.493591, -0.0669488], 1.0e-4, 1.0e-2)

interval = mySobol.getFirstOrderIndicesInterval()
assert_almost_equal(
    interval.getLowerBound(), [0.107531, 0.406036, -0.195595], 1.0e-4, 1.0e-2
)
assert_almost_equal(
    interval.getUpperBound(), [0.328238, 0.569699, 0.0936075], 1.0e-4, 1.0e-2
)

# Testing that we still get the same results when using setDesign
normal = ot.Normal()
mySobol2 = ot.RankSobolSensitivityAlgorithm(normal.getSample(10),
                                            normal.getSample(10))
ot.RandomGenerator.SetSeed(42)
mySobol2.setDesign(sampleX, sampleY, sampleX.getSize())

indices2 = mySobol2.getFirstOrderIndices()
interval2 = mySobol2.getFirstOrderIndicesInterval()

assert_almost_equal(indices2, indices, 0, ot.SpecFunc.ScalarEpsilon)
assert_almost_equal(
    interval2.getLowerBound(), interval.getLowerBound(), 0,
    ot.SpecFunc.ScalarEpsilon)
assert_almost_equal(
    interval2.getUpperBound(), interval.getUpperBound(), 0,
    ot.SpecFunc.ScalarEpsilon)

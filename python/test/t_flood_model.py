#! /usr/bin/env python

import openturns as ot
from openturns.testing import assert_almost_equal
from openturns.usecases import flood_model as flood_model


ot.TESTPREAMBLE()
ot.PlatformInfo.SetNumericalPrecision(5)

print("Test the FloodModel data class.")
fm = flood_model.FloodModel()

# test parameter
assert fm.dim == 8

# test marginals means
assert_almost_equal(fm.Q.getMean()[0], 1338.1299666394023, 1e-12)
assert_almost_equal(fm.Ks.getMean()[0], 30.001003758482675, 1e-12)
assert_almost_equal(fm.Zv.getMean()[0], 50.0, 1e-12)
assert_almost_equal(fm.Zm.getMean()[0], 55.0, 1e-12)
assert_almost_equal(fm.B.getMean()[0], 300.0, 1e-12)
assert_almost_equal(fm.L.getMean()[0], 5000.0, 1e-12)
assert_almost_equal(fm.Zb.getMean()[0], 55.5, 1e-12)
assert_almost_equal(fm.Hd.getMean()[0], 3.0, 1e-12)

meanInput = fm.distribution.getMean()
print("meanInput = ", meanInput)
meanOutput = fm.model(meanInput)
print("meanOutput = ", meanOutput)
assert_almost_equal(meanOutput, [2.5313, -5.96869, 1.0362], 1.0e-4)

#
print("Test the simplified height model")
heightInputDistribution, heightModel = fm.getHeightModel()
meanInput = heightInputDistribution.getMean()
print("meanInput = ", meanInput)
meanOutput = heightModel(meanInput)
print("meanOutput = ", meanOutput)
assert_almost_equal(meanOutput, [2.5313])

print("Test the high Hd scenario")
fm = flood_model.FloodModel(distributionHdLow=False)
assert_almost_equal(fm.Hd.getMean()[0], 8.0, 1e-12)

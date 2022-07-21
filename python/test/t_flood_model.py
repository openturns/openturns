#! /usr/bin/env python

import openturns as ot
from openturns.testing import assert_almost_equal
from openturns.usecases import flood_model as flood_model
from math import pi


ot.TESTPREAMBLE()
ot.PlatformInfo.SetNumericalPrecision(5)

"""
Test the import of the FloodModel data class.
"""
fm = flood_model.FloodModel()

# test parameter
assert_almost_equal(fm.dim, 4, 1e-12)

# test marginals means
assert_almost_equal(fm.Q.getMean()[0], 1338.1299666394023, 1e-12)
assert_almost_equal(fm.Ks.getMean()[0], 30.001003758482675, 1e-12)
assert_almost_equal(fm.Zv.getMean()[0], 50.0, 1e-12)
assert_almost_equal(fm.Zm.getMean()[0], 55.0, 1e-12)

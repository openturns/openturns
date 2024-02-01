#! /usr/bin/env python

import openturns as ot
from openturns.testing import assert_almost_equal
from openturns.usecases import deflection_tube


ot.TESTPREAMBLE()
ot.PlatformInfo.SetNumericalPrecision(5)

"""
Test the import of the DeflectionTube data class.
"""
dt = deflection_tube.DeflectionTube()

assert_almost_equal(dt.dim, 6, 1e-12)

# marginal means
assert_almost_equal(dt.XF.getMean()[0], 1.0, 1e-12)
assert_almost_equal(dt.XE.getMean()[0], 200000.0, 1e-12)
assert_almost_equal(dt.XL.getMean()[0], 1.5, 1e-12)
assert_almost_equal(dt.Xa.getMean()[0], 1.0, 1e-12)
assert_almost_equal(dt.XD.getMean()[0], 0.8, 1e-12)
assert_almost_equal(dt.Xd.getMean()[0], 0.1, 1e-12)

#! /usr/bin/env python

import openturns as ot
from openturns.testing import assert_almost_equal
from openturns.usecases import viscous_free_fall


ot.TESTPREAMBLE()
ot.PlatformInfo.SetNumericalPrecision(5)

"""
Test the import of the Ackley model data class.
"""
vff = viscous_free_fall.ViscousFreeFall()

# scalar parameters
assert_almost_equal(vff.dim, 4, 1e-12)
assert_almost_equal(vff.tmin, 0.0, 1e-12)
assert_almost_equal(vff.tmax, 12.0, 1e-12)
assert_almost_equal(vff.gridsize, 100, 1e-12)
assert_almost_equal(vff.outputDimension, 1, 1e-12)

# marginals : comparing the means
assert_almost_equal(vff.distZ0.getMean()[0], 125.0, 1e-12)
assert_almost_equal(vff.distV0.getMean()[0], 55.0, 1e-12)
assert_almost_equal(vff.distM.getMean()[0], 80.0, 1e-12)
assert_almost_equal(vff.distC.getMean()[0], 15.0, 1e-12)

#! /usr/bin/env python

import openturns as ot
from openturns.testing import assert_almost_equal
from openturns.usecases import branin_function


ot.TESTPREAMBLE()
ot.PlatformInfo.SetNumericalPrecision(5)

"""
Test the import of the BraninModel data class.
"""
bm = branin_function.BraninModel()

# test parameters
assert_almost_equal(bm.dim, 2, 1e-12)
assert_almost_equal(bm.trueNoiseFunction, 0.1, 1e-12)
assert_almost_equal(bm.lowerbound, ot.Point([0.0] * bm.dim), 1e-12)
assert_almost_equal(bm.upperbound, ot.Point([1.0] * bm.dim), 1e-12)

# test minima
assert_almost_equal(bm.xexact1, ot.Point([0.123895, 0.818329]), 1e-12)
assert_almost_equal(bm.xexact2, ot.Point([0.542773, 0.151666]), 1e-12)
assert_almost_equal(bm.xexact3, ot.Point([0.961652, 0.165000]), 1e-12)

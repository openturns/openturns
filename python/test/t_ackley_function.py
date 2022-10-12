#! /usr/bin/env python

import openturns as ot
from openturns.testing import assert_almost_equal
from openturns.usecases import ackley_function as ackley_function
from math import pi


ot.TESTPREAMBLE()
ot.PlatformInfo.SetNumericalPrecision(5)

"""
Test the import of the AckleyModel data class.
"""
am = ackley_function.AckleyModel()

assert_almost_equal(am.dim, 2, 1e-12)
assert_almost_equal(am.a, 20.0, 1e-12)
assert_almost_equal(am.b, 0.2, 1e-12)
assert_almost_equal(am.c, 2.0 * pi, 1e-12)
assert_almost_equal(am.lowerbound, ot.Point([-15.0] * am.dim), 1e-12)
assert_almost_equal(am.upperbound, ot.Point([15.0] * am.dim), 1e-12)
assert_almost_equal(am.x0, [0.0, 0.0], 1e-12)

X = ot.Point([0.0, 0.0])
assert_almost_equal(am.model(X), [0.0], 1e-12)

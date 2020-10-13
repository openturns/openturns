#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
from openturns.testing import assert_almost_equal
from openturns.usecases import ishigami_function as ishigami_function
from math import pi


ot.TESTPREAMBLE()
ot.PlatformInfo.SetNumericalPrecision(5)

"""
Test the import of the IshigamiModel data class.
"""
im = ishigami_function.IshigamiModel()
a = 7.0
b = 0.1

# parameters
assert_almost_equal(im.dim, 3, 1e-12)
assert_almost_equal(im.a, a, 1e-12)
assert_almost_equal(im.b, b, 1e-12)

# marginals means
assert_almost_equal(im.X1.getMean()[0], 0.0, 1e-12)
assert_almost_equal(im.X2.getMean()[0], 0.0, 1e-12)
assert_almost_equal(im.X3.getMean()[0], 0.0, 1e-12)

# special value
X = ot.Point([pi/2, pi/2, 1.0])
assert_almost_equal(im.model(X), [1.0 + a + b], 1e-12)

# First moments
expectation = a / 2.0
assert_almost_equal(im.expectation, expectation, 1e-12)

variance = 1.0/2 + a**2/8.0 + b*pi**4/5.0 + b**2*pi**8/18.0
assert_almost_equal(im.variance, variance, 1e-12)


# Sobol indices
S1 = (1.0/2.0 + b*pi**4/5.0+b**2*pi**8/50.0)/variance
assert_almost_equal(im.S1, S1, 1e-12)

S2 = (a**2/8.0)/variance
assert_almost_equal(im.S2, S2, 1e-12)

assert_almost_equal(im.S3, 0.0, 1e-12)
assert_almost_equal(im.S12, 0.0, 1e-12)

S13 = b**2*pi**8/2.0*(1.0/9.0-1.0/25.0)/variance
assert_almost_equal(im.S13, S13, 1e-12)

S23  = 0.0
assert_almost_equal(im.S23, 0.0, 1e-12)

S123  = 0.0
assert_almost_equal(im.S123, 0.0, 1e-12)

ST1  = S1 + S13
assert_almost_equal(im.ST1, ST1, 1e-12)

ST2  = S2
assert_almost_equal(im.ST2, ST2, 1e-12)

ST3 = S13
assert_almost_equal(im.ST3, ST3, 1e-12)

#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
from openturns.testing import assert_almost_equal
from openturns.usecases import chaboche_model as chaboche_model
from math import pi


ot.TESTPREAMBLE()
ot.PlatformInfo.SetNumericalPrecision(5)

"""
Test the import of the ChabocheModel data class.
"""
cm = chaboche_model.ChabocheModel()

# test parameters
assert_almost_equal(cm.dim, 4, 1e-12)
assert_almost_equal(cm.unknownR, 750.0e6, 1e-12)
assert_almost_equal(cm.unknownC, 2750.0e6, 1e-12)
assert_almost_equal(cm.unknownGamma, 10.0, 1e-12)

# test marginals means
assert_almost_equal(cm.Strain.getMean()[0], 0.035, 1e-12)

assert_almost_equal(cm.R.getMean()[0], 750.0e6, 1e-12)
assert_almost_equal(cm.C.getMean()[0], 2750.0e6, 1e-12)
assert_almost_equal(cm.Gamma.getMean()[0], 10.0, 1e-12)

# special value
X = ot.Point([1.0, 1.0, 1.0, 1.0])
assert_almost_equal(cm.model(X)[0], 1.6321, 1e-3)

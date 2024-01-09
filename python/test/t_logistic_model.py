#! /usr/bin/env python

import openturns as ot
from openturns.testing import assert_almost_equal
from openturns.usecases import logistic_model


ot.TESTPREAMBLE()
ot.PlatformInfo.SetNumericalPrecision(5)

"""
Test the import of the LogisticModel data class.
"""
lm = logistic_model.LogisticModel()

# test parameters
assert_almost_equal(lm.y0, 3.9e6, 1e-12)
assert_almost_equal(lm.a, 0.03134, 1e-12)
assert_almost_equal(lm.b, 1.5887e-10, 1e-12)

# test marginals means
assert_almost_equal(lm.distY0.getMean()[0], 3.9e6, 1e-12)
assert_almost_equal(lm.distA.getMean()[0], 0.03134, 1e-12)
assert_almost_equal(lm.distB.getMean()[0], 1.5887e-10, 1e-12)

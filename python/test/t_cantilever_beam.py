#! /usr/bin/env python

import openturns as ot
from openturns.testing import assert_almost_equal
from openturns.usecases import cantilever_beam as cantilever_beam


ot.TESTPREAMBLE()
ot.PlatformInfo.SetNumericalPrecision(5)

"""
Test the import of the CantileverBeam data class.
"""
cb = cantilever_beam.CantileverBeam()

# test parameters
assert_almost_equal(cb.dim, 4, 1e-12)
assert_almost_equal(cb.R[2, 3], -0.2, 1e-12)

# test marginals means
assert_almost_equal(cb.E.getMean()[0], 67045454545.454544, 1e-12)
assert_almost_equal(cb.F.getMean()[0], 299.99999999999994, 1e-12)
assert_almost_equal(cb.I.getMean()[0], 1.4538461538461538e-07, 1e-12)
assert_almost_equal(cb.L.getMean()[0], 2.55, 1e-12)

# special value
X = ot.Point([1.0, 3.0, 1.0, 1.0])
assert_almost_equal(cb.model(X), [1.0], 1e-12)

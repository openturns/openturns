#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
from openturns.testing import assert_almost_equal
from openturns.usecases import stressed_beam as stressed_beam
from math import pi


ot.TESTPREAMBLE()
ot.PlatformInfo.SetNumericalPrecision(5)

"""
Test the import of the AxialStressedBeam data class.
"""
sb = stressed_beam.AxialStressedBeam()


# test parameters
assert_almost_equal(sb.D, 0.02, 1e-12)
assert_almost_equal(sb.muR, 3.0e6, 1e-12)
assert_almost_equal(sb.sigmaR, 3.0e5, 1e-12)
assert_almost_equal(sb.muF, 750.0, 1e-12)
assert_almost_equal(sb.sigmaF, 50.0, 1e-12)

# test marginals means
assert_almost_equal(sb.distribution_R.getMean()[0],3.0e6,1e-12)
assert_almost_equal(sb.distribution_F.getMean()[0],750.0,1e-12)

# special value of the model function
X = ot.Point([1.0,pi/10000.0])
assert_almost_equal(sb.model(X), [0.0], 1e-12)

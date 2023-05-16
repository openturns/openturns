#! /usr/bin/env python

import openturns as ot
from openturns.testing import assert_almost_equal
from openturns.usecases import fireSatellite_function


ot.TESTPREAMBLE()
ot.PlatformInfo.SetNumericalPrecision(5)

"""
Test the import of the FireSatellite model data class.
"""
m = fireSatellite_function.FireSatelliteModel()

assert_almost_equal(m.model(m.distributionX.getMean())[0], 0.011659, 1e-2)

# test getMarginal (if serialization is available)
if ot.PlatformInfo.HasFeature("libxml2"):
    m0 = m.model.getMarginal(0)

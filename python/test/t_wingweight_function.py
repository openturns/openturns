#! /usr/bin/env python

import openturns as ot
from openturns.testing import assert_almost_equal
from openturns.usecases import wingweight_function as wingweight_function

ot.TESTPREAMBLE()
ot.PlatformInfo.SetNumericalPrecision(5)

"""
Test the WingWeightModel data class on a reference point.
"""
ww = wingweight_function.WingWeightModel()


assert_almost_equal(ww.model(ww.distributionX.getMean()), [267.625], 1e-2)

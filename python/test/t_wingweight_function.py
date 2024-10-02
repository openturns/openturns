#! /usr/bin/env python

from openturns.testing import assert_almost_equal
from openturns.usecases import wingweight_function

"""
Test the WingWeightModel data class on a reference point.
"""
ww = wingweight_function.WingWeightModel()


assert_almost_equal(ww.model(ww.inputDistribution.getMean()), [267.625], 1e-2)

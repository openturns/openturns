#! /usr/bin/env python

from openturns.testing import assert_almost_equal
from openturns.usecases import oscillator

"""
Test the WingWeightModel data class on a reference point.
"""
osc = oscillator.Oscillator()


assert_almost_equal(osc.model(osc.distribution.getMean()), [23.1897], 1e-2)

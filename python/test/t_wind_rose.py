#! /usr/bin/env python

import math
import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott
from openturns.usecases import wind_rose

ot.TESTPREAMBLE()

use_case = wind_rose.WindRose()
data = use_case.directions
print(data.getSize(), data.getDimension())
assert data.getSize() == 365
assert data.getDimension() == 1

# All the headings lie in [0, 2 pi)
for i in range(data.getSize()):
    assert 0.0 <= data[i, 0] < 2.0 * math.pi

# WrappedNormal factory recovers the mean heading and the dispersion
estimated = otexp.WrappedNormalFactory().build(data).getImplementation()
mu = estimated.getMu()[0]
ref = use_case.getReferenceMu()
gap = abs((mu - ref + math.pi) % (2.0 * math.pi) - math.pi)
ott.assert_almost_equal(gap, 0.0, 0.0, 0.15)
ott.assert_almost_equal(estimated.getSigma()[0, 0], use_case.getReferenceSigma2(), 0.0, 0.3)

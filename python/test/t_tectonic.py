#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott
from openturns.usecases import tectonic

ot.TESTPREAMBLE()

use_case = tectonic.Tectonic()
data = use_case.directions
print(data.getSize(), data.getDimension())
assert data.getSize() == 150
assert data.getDimension() == 3

# All the directions are unit vectors
for i in range(data.getSize()):
    ott.assert_almost_equal(data[i].norm(), 1.0, 1e-12, 0.0)

# Kent factory recovers the concentration and ovalness
estimated = otexp.KentFactory().build(data).getImplementation()
ott.assert_almost_equal(estimated.getKappa(), use_case.getReferenceKappa(), 0.0, 3.0)
ott.assert_almost_equal(estimated.getBeta(), use_case.getReferenceBeta(), 0.0, 1.5)

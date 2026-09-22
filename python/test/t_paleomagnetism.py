#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott
from openturns.usecases import paleomagnetism

ot.TESTPREAMBLE()

use_case = paleomagnetism.Paleomagnetism()
data = use_case.directions
print(data.getSize(), data.getDimension())
assert data.getSize() == 150
assert data.getDimension() == 3

# All the directions are unit vectors
for i in range(data.getSize()):
    ott.assert_almost_equal(data[i].norm(), 1.0, 1e-12, 0.0)

# Fisher factory recovers the reference direction and concentration
estimated = otexp.VonMisesFisherFactory().build(data).getImplementation()
mu = estimated.getMu()
ref = use_case.getReferenceMeanDirection()
ott.assert_almost_equal(mu[0] * ref[0] + mu[1] * ref[1] + mu[2] * ref[2], 1.0, 1e-3, 0.0)
ott.assert_almost_equal(estimated.getKappa(), use_case.getReferenceKappa(), 0.0, 2.0)

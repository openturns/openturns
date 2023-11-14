#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

# Latent variable model for 4 categorical levels and
# a 3-dimensional latent space
k = otexp.LatentVariableModel(4, 3)
k.setLatentVariables([0.1, 0.2, 0.3, -0.1, -0.2, -0.3, 0.4])
k.setScale([1.5])
k.setAmplitude([2.0])

# We define a squared exponential kernel in the latent space as a reference
kRef = ot.SquaredExponential(3)
kRef.setScale(ot.Point(3, 1.5))
kRef.setAmplitude(ot.Point(1, 2.0))

ott.assert_almost_equal(
    k(1, 1)[0, 0], kRef(ot.Point([0.1, 0.0, 0.0]), ot.Point([0.1, 0.0, 0.0]))[0, 0]
)
ott.assert_almost_equal(
    k(1, 2)[0, 0], kRef(ot.Point([0.1, 0.0, 0.0]), ot.Point([0.2, 0.3, -0.1]))[0, 0]
)
ott.assert_almost_equal(
    k(0, 3)[0, 0], kRef(ot.Point([0.0, 0.0, 0.0]), ot.Point([-0.2, -0.3, 0.4]))[0, 0]
)

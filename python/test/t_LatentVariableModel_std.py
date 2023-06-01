#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(0)

# Latent variable model for 4 categorical levels and
# a 3-dimensional latent space
k = otexp.LatentVariableModel(4, 3)
k.setLatentVariables([0.1, 0.2, 0.3, -0.1, -0.2, -0.3, 0.4])
k.setScale([1.5])
k.setAmplitude([2.0])
ott.assert_almost_equal(k(1, 1)[0, 0], 4.0)
ott.assert_almost_equal(k(1, 2)[0, 0], 3.903408)
ott.assert_almost_equal(k(0, 3)[0, 0], 3.750353)

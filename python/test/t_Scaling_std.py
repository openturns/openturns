#! /usr/bin/env python

"""Test the MuSigmaScaling and MinMaxScaling classes, see issue #3104."""

import os

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

# MuSigmaScaling
mu = [1.0, 2.0]
sigma = [2.0, 4.0]
scaling = otexp.MuSigmaScaling(mu, sigma)
ott.assert_almost_equal(scaling.getMu(), mu)
ott.assert_almost_equal(scaling.getSigma(), sigma)
forward = scaling.getTransform()
backward = scaling.getInverseTransform()
point = [3.0, 6.0]
ott.assert_almost_equal(forward(point), [1.0, 1.0])
ott.assert_almost_equal(backward([1.0, 1.0]), point)
ott.assert_almost_equal(backward(forward(point)), point)
with ott.assert_raises(TypeError):
    otexp.MuSigmaScaling([1.0, 2.0], [2.0, 0.0])
with ott.assert_raises(TypeError):
    otexp.MuSigmaScaling([1.0], [2.0, 4.0])

# MinMaxScaling
minimum = [1.0, 2.0]
maximum = [3.0, 6.0]
minmax = otexp.MinMaxScaling(minimum, maximum)
ott.assert_almost_equal(minmax.getMinimum(), minimum)
ott.assert_almost_equal(minmax.getMaximum(), maximum)
ott.assert_almost_equal(minmax.getTransform()([2.0, 4.0]), [0.5, 0.5])
ott.assert_almost_equal(minmax.getInverseTransform()([0.5, 0.5]), [2.0, 4.0])
ott.assert_almost_equal(
    minmax.getInverseTransform()(minmax.getTransform()([2.0, 4.0])), [2.0, 4.0]
)
with ott.assert_raises(TypeError):
    otexp.MinMaxScaling([1.0, 2.0], [1.0, 6.0])

# persistence round-trip
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("scaling.xml"))
study.add("musigma", scaling)
study.add("minmax", minmax)
study.save()
loaded = ot.Study()
loaded.setStorageManager(ot.XMLStorageManager("scaling.xml"))
loaded.load()
assert loaded.hasObject("musigma")
assert loaded.hasObject("minmax")
os.remove("scaling.xml")

# Non-finite scaling parameters are rejected
with ott.assert_raises(Exception):
    otexp.MinMaxScaling([-1.0e308], [1.0e308])
with ott.assert_raises(Exception):
    otexp.MinMaxScaling([0.0], [float("inf")])
with ott.assert_raises(Exception):
    otexp.MinMaxScaling([0.0], [1.0e-310])
with ott.assert_raises(Exception):
    otexp.MuSigmaScaling([float("inf")], [1.0])
with ott.assert_raises(Exception):
    otexp.MuSigmaScaling([0.0], [float("inf")])
with ott.assert_raises(Exception):
    otexp.MuSigmaScaling([0.0], [1.0e-310])

print("All tests passed!")

#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import os

ot.TESTPREAMBLE()

# Latent variable model for 4 categorical levels and
# a 3-dimensional latent space
k = ot.LatentVariableModel(4, 3)
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

# Extended deterministic coverage of accessors and error paths
with ott.assert_raises(Exception):
    ot.LatentVariableModel(4, 0)
with ott.assert_raises(Exception):
    ot.LatentVariableModel(1, 2)

defaultModel = ot.LatentVariableModel(4, 3)
assert "LatentVariableModel" in repr(defaultModel)
assert "LatentVariableModel" in str(defaultModel)
assert defaultModel.getLatentDimension() == 3
assert defaultModel.getLevelNumber() == 4
assert defaultModel.getFullLatentVariables().getSize() == 4
assert defaultModel.getActiveLatentVariables() == ot.Point(7, 0.0)
assert defaultModel.getFullParameter().getSize() == 10
assert defaultModel.getFullParameterDescription().getSize() == 10
assert defaultModel == ot.LatentVariableModel(4, 3)
assert not (defaultModel == k)
assert defaultModel != k

with ott.assert_raises(Exception):
    defaultModel.setLatentVariables([0.1])
with ott.assert_raises(Exception):
    k(0.5, 1)
with ott.assert_raises(Exception):
    k(0, 7)
with ott.assert_raises(Exception):
    defaultModel.setFullParameter([1.0])
fullParam = k.getFullParameter()
k.setFullParameter(fullParam)
ott.assert_almost_equal(k.getFullParameter(), fullParam)
badParam = ot.Point(fullParam)
badParam[0] = -1.0
with ott.assert_raises(Exception):
    k.setFullParameter(badParam)
badParam = ot.Point(fullParam)
badParam[1] = -1.0
with ott.assert_raises(Exception):
    k.setFullParameter(badParam)
badParam = ot.Point(fullParam)
badParam[2] = -1.0
with ott.assert_raises(Exception):
    k.setFullParameter(badParam)
with ott.assert_raises(Exception):
    k.setScale([1.0, 2.0])
with ott.assert_raises(Exception):
    k.setScale([-1.0])
with ott.assert_raises(Exception):
    k.setAmplitude([1.0, 2.0])
with ott.assert_raises(Exception):
    k.setAmplitude([-1.0])
with ott.assert_raises(Exception):
    k.setNuggetFactor(-0.5)
k.setNuggetFactor(0.1)
assert k.getNuggetFactor() == 0.1
with ott.assert_raises(Exception):
    k.draw()

# persistence roundtrip (regression: load dropped latent state)

study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("latent_variable_model.xml"))
study.add("model", ot.CovarianceModel(k))
study.save()
loadedStudy = ot.Study()
loadedStudy.setStorageManager(ot.XMLStorageManager("latent_variable_model.xml"))
loadedStudy.load()
loaded = ot.CovarianceModel()
loadedStudy.fillObject("model", loaded)
assert loaded == k
ott.assert_almost_equal(loaded([0.0], [1.0]), k([0.0], [1.0]), 1e-12, 1e-12)
os.remove("latent_variable_model.xml")

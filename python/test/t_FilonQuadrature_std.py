#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import math
import os

ot.TESTPREAMBLE()

f = ot.SymbolicFunction(["t"], ["log(1+t)"])
a = 0.5
b = 0.5 + 8.0 * math.pi
n = 100
omega = 1000.0
kind = 0
algoF = ot.FilonQuadrature(n, omega, kind)
value = algoF.integrate(f, ot.Interval(a, b))
print(value)
kind = 1
algoF = ot.FilonQuadrature(n, omega, kind)
value = algoF.integrate(f, ot.Interval(a, b))
print(value)
kind = 2
algoF = ot.FilonQuadrature(n, omega, kind)
value = algoF.integrate(f, ot.Interval(a, b))
print(value)

# Extended deterministic coverage (no prints)
defaultAlgo = ot.FilonQuadrature()
assert defaultAlgo.getN() == 1
ott.assert_almost_equal(defaultAlgo.getOmega(), 1.0)
assert defaultAlgo.getKind() == 0
assert "FilonQuadrature" in repr(defaultAlgo)
assert "FilonQuadrature" in str(defaultAlgo)
assert "cos" in str(ot.FilonQuadrature(1, 1.0, 0))
assert "sin" in str(ot.FilonQuadrature(1, 1.0, 1))
assert "exp" in str(ot.FilonQuadrature(1, 1.0, 2))
with ott.assert_raises(Exception):
    ot.FilonQuadrature(0, 1.0, 0)
algo = ot.FilonQuadrature(10, 1.0, 0)
with ott.assert_raises(Exception):
    algo.setN(0)
algo.setN(20)
algo.setOmega(2.0)
algo.setKind(1)
assert algo.getN() == 20
ott.assert_almost_equal(algo.getOmega(), 2.0)
assert algo.getKind() == 1
# small-theta series branch
smallCos = ot.FilonQuadrature(5, 0.01, 0)
ott.assert_almost_equal(smallCos.integrate(f, ot.Interval(0.5, 1.5)), [0.682488], 1e-4, 1e-4)
smallSin = ot.FilonQuadrature(5, 0.01, 1)
ott.assert_almost_equal(smallSin.integrate(f, ot.Interval(0.5, 1.5)), [0.00724715], 1e-4, 1e-6)
smallExp = ot.FilonQuadrature(5, 0.01, 2)
assert smallExp.integrate(f, ot.Interval(0.5, 1.5)).getDimension() == 2
with ott.assert_raises(Exception):
    algo.integrate(f, ot.Interval([0.0, 0.0], [1.0, 1.0]))
with ott.assert_raises(Exception):
    algo.integrate(ot.SymbolicFunction(["x", "y"], ["x+y"]), ot.Interval(0.0, 1.0))

study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("filon_quadrature.xml"))
study.add("algo", algo)
study.save()
loadedStudy = ot.Study()
loadedStudy.setStorageManager(ot.XMLStorageManager("filon_quadrature.xml"))
loadedStudy.load()
loaded = ot.FilonQuadrature()
loadedStudy.fillObject("algo", loaded)
assert loaded.getN() == algo.getN()
ott.assert_almost_equal(loaded.getOmega(), algo.getOmega())
assert loaded.getKind() == algo.getKind()
os.remove("filon_quadrature.xml")

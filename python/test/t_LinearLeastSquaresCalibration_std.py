#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing as ott
import math as m
import sys

ot.TESTPREAMBLE()
ot.PlatformInfo.SetNumericalPrecision(5)

m = 10
x = [[0.5 + i] for i in range(m)]

inVars = ["a", "b", "c", "x"]
formulas = ["a + b * exp(c * x)", "(a * x^2 + b) / (c + x^2)"]
model = ot.SymbolicFunction(inVars, formulas)
p_ref = [2.8, 1.2, 0.5]
params = [0, 1, 2]
modelX = ot.ParametricFunction(model, params, p_ref)
y = modelX(x)
y += ot.Normal([0.0]*2, [0.05]*2, ot.IdentityMatrix(2)).getSample(m)
candidate = [1.0]*3
methods = ["SVD", "QR", "Cholesky"]
for method in methods:
    print("method=", method)
    algo = ot.LinearLeastSquaresCalibration(modelX, x, y, candidate, method)
    algo.run()
    print("result=", algo.getResult())

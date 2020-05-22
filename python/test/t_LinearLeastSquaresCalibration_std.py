#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()
ot.PlatformInfo.SetNumericalPrecision(5)

m = 10
x = [[0.5 + i] for i in range(m)]

inVars = ["a", "b", "c", "x"]
formulas = ["a + b * exp(c * x)", "(a * x^2 + b) / (c + x^2)"]
g = ot.SymbolicFunction(inVars, formulas)
trueParameter = [2.8, 1.2, 0.5]
params = [0, 1, 2]
model = ot.ParametricFunction(g, params, trueParameter)
y = model(x)
y += ot.Normal([0.0] * 2, [0.05] * 2, ot.IdentityMatrix(2)).getSample(m)
candidate = [1.0] * 3
methods = ["SVD", "QR", "Cholesky"]
for method in methods:
    print("method=", method)
    algo = ot.LinearLeastSquaresCalibration(model, x, y, candidate, method)
    algo.run()
    print("result=", algo.getResult())

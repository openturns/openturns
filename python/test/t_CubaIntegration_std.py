#! /usr/bin/env python

import openturns as ot
import math
import openturns.testing as ott

ot.TESTPREAMBLE()

# Testing four algorithms on a function from R^3 to R.
f = ot.SymbolicFunction(["x", "y", "z"], ["sin(x) * cos(y) * exp(z)"])
lbIntegration = [0.0, 0.0, 0.0]
ubIntegration = [1.0, 1.0, 1.0]
valueRef = -math.sin(1.0) * (math.cos(1.0) - 1.0) * (math.e - 1.0)

for r in ot.CubaIntegration.GetAlgorithmNames():
    algoC = ot.CubaIntegration(r)
    value = algoC.integrate(f, ot.Interval(lbIntegration, ubIntegration))
    ott.assert_almost_equal(value[0], valueRef, 1.0e-3, 1.0e-3)

# Testing three algorithms on a function from R^3 to R^2.
f = ot.SymbolicFunction(
    ["x", "y", "z"],
    [
        "exp(-(x * x + y * y + z *z))",
        "(x / 2 * x / 2 + y / 3 * y / 3 + z / 4 * z / 4 - 1.125)",
    ],
)
lbIntegration = [0.0, 0.0, 1.0]
ubIntegration = [2.0, 3.0, 4.0]
valueRef = [0.108972129575688278, -0.375]

for r in ot.CubaIntegration.GetAlgorithmNames():
    if r == "suave":
        # Suave is quite inaccurate with these integrands, skipping.
        continue
    algoC = ot.CubaIntegration(r)
    if r == "vegas":
        # Vegas seemingly needs more favorable parameters there
        algoC.setMaximumRelativeError(5.0e-3)
        algoC.setMaximumAbsoluteError(5.0e-3)
        algoC.setMaximumCallsNumber(10000000)
    value = algoC.integrate(f, ot.Interval(lbIntegration, ubIntegration))
    ott.assert_almost_equal(value, valueRef, 1.0e-3, 1.0e-3)

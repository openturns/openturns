#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott


def f_py(x):
    if x[0] < 0.0:
        raise RuntimeError("negative input")
    return [x[0] ** 0.5]


def df_py(x):
    if x[0] < 0.0:
        raise RuntimeError("negative input")
    return [[0.5 / x[0] ** 0.5]]


f = ot.PythonFunction(1, 1, f_py, gradient=df_py)
g = otexp.PenalizedGradient(f.getGradient())

# gradient on success
ott.assert_almost_equal(g.gradient([4.0])[0, 0], 0.25, 1e-12, 0.0)
# gradient on failure returns zeros
ott.assert_almost_equal(g.gradient([-1.0]), ot.Matrix(1, 1), 0.0, 0.0)

# accessors
assert g.getGradient().getInputDimension() == 1
g.setGradient(f.getGradient())
assert g.getGradient().getOutputDimension() == 1

# marginal keeps penalization
h = otexp.PenalizedGradient(
    ot.SymbolicFunction(["x1", "x2"], ["x1", "x2"]).getGradient()
)
identity = ot.Matrix(2, 2)
identity[0, 0] = 1.0
identity[1, 1] = 1.0
ott.assert_almost_equal(h.gradient([1.0, 2.0]), identity, 1e-12, 0.0)
h1 = h.getMarginal(1)
assert h1.getImplementation().getClassName() == "PenalizedGradient"
marginal = ot.Matrix(2, 1)
marginal[1, 0] = 1.0
ott.assert_almost_equal(h1.gradient([1.0, 2.0]), marginal, 1e-12, 0.0)

# dimensions and actual implementation flag
assert g.getInputDimension() == 1
assert g.getOutputDimension() == 1
assert g.isActualImplementation()

# calls number
before = g.getCallsNumber()
g.gradient([4.0])
g.gradient([-1.0])
assert g.getCallsNumber() == before + 2

print("ok")

#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

dim = 3
f = ot.IdentityFunction(dim)
print(f)

assert f.getInputDimension() == dim, "input dim"
assert f.getOutputDimension() == dim, "output dim"

x = ot.Point([i + 1.0 for i in range(dim)])
y = f(x)
print(x, y)
assert x == y, "wrong result"

sampleIn = ot.Sample([[1.0, 2.0, 3.0], [4.0, 5.0, 6.0]])
ott.assert_almost_equal(f(sampleIn), sampleIn, 1e-14, 1e-14)
ott.assert_almost_equal(
    f.getGradient().gradient(x),
    ot.Matrix(
        [[1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]]
    ),
    1e-14,
    1e-14,
)
_ = f.getHessian().hessian(x)
assert f.getCallsNumber() > 0, "calls"
assert f.getEvaluationCallsNumber() > 0, "evaluation calls"
assert f.getGradientCallsNumber() > 0, "gradient calls"
assert f.getHessianCallsNumber() > 0, "hessian calls"
assert f.getEvaluation().isLinear(), "linearity"
assert f.getEvaluation().isLinearlyDependent(0), "dependence"
_ = str(f)
_ = repr(f)
assert f == ot.IdentityFunction(dim), "equality"
assert f != ot.IdentityFunction(dim + 1), "inequality"
with ott.assert_raises(Exception):
    ot.IdentityFunction(0)
with ott.assert_raises(Exception):
    f([1.0])
with ott.assert_raises(Exception):
    f([[1.0, 2.0]])
with ott.assert_raises(Exception):
    f.getGradient().gradient([1.0])
with ott.assert_raises(Exception):
    f.getHessian().hessian([1.0])
with ott.assert_raises(Exception):
    f.getEvaluation().isLinearlyDependent(dim + 1)

#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott


def f_py(x):
    if x[0] < 0.0:
        raise RuntimeError("negative input")
    return [x[0] ** 2]


def d2f_py(x):
    if x[0] < 0.0:
        raise RuntimeError("negative input")
    return [[[2.0]]]


f = ot.PythonFunction(1, 1, f_py, hessian=d2f_py)
h = otexp.PenalizedHessian(f.getHessian())

# hessian on success
ott.assert_almost_equal(h.hessian([4.0])[0, 0, 0], 2.0, 1e-12, 0.0)
# hessian on failure returns zeros
ott.assert_almost_equal(h.hessian([-1.0]), ot.SymmetricTensor(1, 1), 0.0, 0.0)

# accessors
assert h.getHessian().getInputDimension() == 1
h.setHessian(f.getHessian())
assert h.getHessian().getOutputDimension() == 1

# coordinated evaluation: zeros where only the evaluation throws


def d2f_ok_py(x):
    return [[[2.0]]]


f2 = ot.PythonFunction(1, 1, f_py, hessian=d2f_ok_py)
hc = otexp.PenalizedHessian(f2.getHessian(), f2.getEvaluation())
ott.assert_almost_equal(hc.hessian([4.0])[0, 0, 0], 2.0, 1e-12, 0.0)
ott.assert_almost_equal(
    hc.hessian([-1.0]), ot.SymmetricTensor(1, 1), 0.0, 0.0
)
assert hc.getEvaluation().getInputDimension() == 1
hc.setEvaluation(f2.getEvaluation())
# mismatched dimensions are rejected
with ott.assert_raises(TypeError):
    hc.setEvaluation(ot.SymbolicFunction(["x1", "x2"], ["x1"]).getEvaluation())

# marginal keeps penalization
g = otexp.PenalizedHessian(
    ot.SymbolicFunction(["x1", "x2"], ["x1", "x2"]).getHessian()
)
null = ot.SymmetricTensor(2, 2)
ott.assert_almost_equal(g.hessian([1.0, 2.0]), null, 0.0, 0.0)
g1 = g.getMarginal(1)
assert g1.getImplementation().getClassName() == "PenalizedHessian"
ott.assert_almost_equal(g1.hessian([1.0, 2.0]), ot.SymmetricTensor(2, 1))

# dimensions and actual implementation flag
assert h.getInputDimension() == 1
assert h.getOutputDimension() == 1
assert h.isActualImplementation()

# calls number
before = h.getCallsNumber()
h.hessian([4.0])
h.hessian([-1.0])
assert h.getCallsNumber() == before + 2

print("ok")

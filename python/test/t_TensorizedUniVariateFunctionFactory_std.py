#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

# Polynomial factories
factoryCollection = [ot.MonomialFunctionFactory()] * 3

dim = len(factoryCollection)
factory = ot.TensorizedUniVariateFunctionFactory(factoryCollection)
print("factory=", factory)
x = [0.5, 1.0, 1.5]
for i in range(10):
    f = factory.build(i)
    print("i=", i, "f(X)=", f(x))

# Exercise ProductUniVariateFunctionEvaluation/Gradient/Hessian
for i in range(5):
    f = factory.build(i)
    point = ot.Point(x)
    value = f(point)
    ott.assert_almost_equal(f(x), value)
    sample = ot.Sample([x, x])
    ott.assert_almost_equal(f(sample), ot.Sample([value, value]))
    evaluation = f.getEvaluation()
    ott.assert_almost_equal(evaluation(point), value)
    ott.assert_almost_equal(evaluation(sample), ot.Sample([value, value]))
    assert evaluation.getInputDimension() == dim
    assert evaluation.getOutputDimension() == 1
    assert "ProductUniVariateFunctionEvaluation" in repr(evaluation)
    str(evaluation)
    gradient = f.getGradient()
    ott.assert_almost_equal(f.gradient(point), gradient.gradient(point))
    assert gradient.getInputDimension() == dim
    assert gradient.getOutputDimension() == 1
    assert "Gradient" in repr(gradient)
    hessian = f.getHessian()
    ott.assert_almost_equal(f.hessian(point), hessian.hessian(point))
    assert hessian.getInputDimension() == dim
    assert hessian.getOutputDimension() == 1
    assert "Hessian" in repr(hessian)
    assert f == f
    assert not (f != f)
    assert evaluation == evaluation
    assert not (evaluation != evaluation)
    with ott.assert_raises(Exception):
        f([0.5])
    with ott.assert_raises(Exception):
        f(ot.Sample([[0.5] * (dim + 1)] * 2))
    with ott.assert_raises(Exception):
        evaluation([0.5])
    with ott.assert_raises(Exception):
        evaluation(ot.Sample([[0.5] * (dim + 1)] * 2))
    with ott.assert_raises(Exception):
        f.gradient([0.5])
    with ott.assert_raises(Exception):
        f.hessian([0.5])
    with ott.assert_raises(Exception):
        gradient.gradient([0.5])
    with ott.assert_raises(Exception):
        hessian.hessian([0.5])
assert factory.build(0) != factory.build(1)

# Zero-product branch of gradient/hessian: build(1) vanishes at x0 == 0
zeroPoint = [0.0, 1.0, 1.5]
fOne = factory.build(1)
ott.assert_almost_equal(fOne(zeroPoint), [0.0])
fOne.gradient(zeroPoint)
fOne.hessian(zeroPoint)

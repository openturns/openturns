#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Polynomial factories
factoryCollection = [
    ot.LaguerreFactory(2.5),
    ot.LegendreFactory(),
    ot.HermiteFactory(),
]
dim = len(factoryCollection)
basisFactory = ot.OrthogonalProductPolynomialFactory(factoryCollection)
print("basisFactory=")
print(basisFactory)
print(basisFactory.__repr_markdown__())
print(basisFactory._repr_html_())

basis = ot.OrthogonalBasis(basisFactory)
print("basis=")
print(basis)

x = [0.5] * dim
enum = basis.getEnumerateFunction()

# Test build by index and build by multi-index produce the same result
for i in range(10):
    f_by_index = basis.build(i)
    indices = enum(i)
    f_by_multiindex = basis.build(indices)
    val_by_index = f_by_index(x)
    val_by_multiindex = f_by_multiindex(x)
    print("i=", i, "f(X)=", val_by_index)
    ott.assert_almost_equal(val_by_index, val_by_multiindex)

# Other factories
factoryCollection2 = [
    ot.OrthogonalUniVariatePolynomialFunctionFactory(ot.LaguerreFactory(2.5)),
    ot.HaarWaveletFactory(),
    ot.FourierSeriesFactory(),
]
dim2 = len(factoryCollection2)
basisFactory2 = ot.OrthogonalProductFunctionFactory(factoryCollection2)
basis2 = ot.OrthogonalBasis(basisFactory2)
print("basis=", basis2)
x2 = [0.5] * dim2
enum2 = basis2.getEnumerateFunction()
for i in range(10):
    f = basis2.build(i)
    print("i=", i, "f(X)=", f(x2))

# Polynomial factories using a collection of distributions
distributionCollection = [
    ot.Normal(),
    ot.TruncatedDistribution(ot.Normal(2.0, 1.5), ot.Interval(1.0, 4.0)),
    ot.Uniform(),
]
basisFactory3 = ot.OrthogonalProductPolynomialFactory(distributionCollection)
print("basisFactory=")
print(basisFactory3)
print(basisFactory3.__repr_markdown__())
print(basisFactory3._repr_html_())

# Exercise ProductUniVariateFunctionEvaluation/Gradient/Hessian via basis2
for i in range(5):
    f = basis2.build(i)
    point = ot.Point(x2)
    value = f(point)
    ott.assert_almost_equal(f(x2), value)
    sample = ot.Sample([x2, x2])
    ott.assert_almost_equal(f(sample), ot.Sample([value, value]))
    evaluation = f.getEvaluation()
    ott.assert_almost_equal(evaluation(point), value)
    ott.assert_almost_equal(evaluation(sample), ot.Sample([value, value]))
    assert evaluation.getInputDimension() == dim2
    assert evaluation.getOutputDimension() == 1
    assert "ProductUniVariateFunctionEvaluation" in repr(evaluation)
    str(evaluation)
    gradient = f.getGradient()
    ott.assert_almost_equal(f.gradient(point), gradient.gradient(point))
    assert gradient.getInputDimension() == dim2
    assert gradient.getOutputDimension() == 1
    assert "Gradient" in repr(gradient)
    hessian = f.getHessian()
    ott.assert_almost_equal(f.hessian(point), hessian.hessian(point))
    assert hessian.getInputDimension() == dim2
    assert hessian.getOutputDimension() == 1
    assert "Hessian" in repr(hessian)
    assert f == f
    assert not (f != f)
    # build from multi-index agrees with build from index
    ott.assert_almost_equal(basis2.build(enum2(i))(x2), value)
    with ott.assert_raises(Exception):
        f([0.5])
    with ott.assert_raises(Exception):
        f(ot.Sample([[0.5] * (dim2 + 1)] * 2))
    with ott.assert_raises(Exception):
        evaluation([0.5])
    with ott.assert_raises(Exception):
        evaluation(ot.Sample([[0.5] * (dim2 + 1)] * 2))
    with ott.assert_raises(Exception):
        f.gradient([0.5])
    with ott.assert_raises(Exception):
        f.hessian([0.5])
    with ott.assert_raises(Exception):
        gradient.gradient([0.5])
    with ott.assert_raises(Exception):
        hessian.hessian([0.5])
assert basis2.build(0) != basis2.build(1)

# Zero-product branch: Haar factor vanishes outside [0, 1]
fZero = basis2.build(3)
zeroPoint = [0.5, 2.0, 0.5]
ott.assert_almost_equal(fZero(zeroPoint), [0.0])
fZero.gradient(zeroPoint)
fZero.hessian(zeroPoint)

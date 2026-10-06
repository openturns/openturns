#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

dimension = 2

# Create the orthogonal basis
enumerateFunction = ot.LinearEnumerateFunction(dimension)
productBasis = ot.OrthogonalProductPolynomialFactory(
    [ot.LegendreFactory(), ot.HermiteFactory()], enumerateFunction
)
print(productBasis)
print("print() :")
for i in range(20):
    p = productBasis.build(i)
    print("type = ", type(p))
    print(p)
    print(p._repr_html_())

# Test build from multi-index
for i in range(20):
    index = enumerateFunction(i)
    termBasis2 = productBasis.build(index)

# Test getMarginal
enumerateFunction = ot.LinearEnumerateFunction(5)
productBasis = ot.OrthogonalProductPolynomialFactory(
    [
        ot.LegendreFactory(),
        ot.HermiteFactory(),
        ot.LegendreFactory(),
        ot.HermiteFactory(),
        ot.HermiteFactory(),
    ],
    enumerateFunction,
)
productBasisMarginal = productBasis.getMarginal([0, 2, 4])
for i in range(20):
    function = productBasisMarginal.build(i)

# Test isTensorProduct()
assert productBasis.isTensorProduct()

# Exercise ProductPolynomialEvaluation/Gradient/Hessian
polyBasis = ot.OrthogonalProductPolynomialFactory(
    [ot.LegendreFactory(), ot.HermiteFactory()], ot.LinearEnumerateFunction(2)
)
point = [0.5, -0.5]
for i in range(5):
    f = polyBasis.build(i)
    value = f(point)
    sample = ot.Sample([point, point])
    ott.assert_almost_equal(f(sample), ot.Sample([value, value]))
    evaluation = f.getEvaluation()
    ott.assert_almost_equal(evaluation(ot.Point(point)), value)
    ott.assert_almost_equal(evaluation(sample), ot.Sample([value, value]))
    assert evaluation.getInputDimension() == dimension
    assert evaluation.getOutputDimension() == 1
    assert "ProductPolynomialEvaluation" in repr(evaluation)
    str(evaluation)
    gradient = f.getGradient()
    ott.assert_almost_equal(f.gradient(point), gradient.gradient(point))
    assert gradient.getInputDimension() == dimension
    assert gradient.getOutputDimension() == 1
    assert "Gradient" in repr(gradient)
    hessian = f.getHessian()
    ott.assert_almost_equal(f.hessian(point), hessian.hessian(point))
    assert hessian.getInputDimension() == dimension
    assert hessian.getOutputDimension() == 1
    assert "Hessian" in repr(hessian)
    assert f == f
    assert not (f != f)
    assert evaluation == evaluation
    assert not (evaluation != evaluation)
    with ott.assert_raises(Exception):
        f([0.5])
    with ott.assert_raises(Exception):
        f(ot.Sample([[0.5] * (dimension + 1)] * 2))
    with ott.assert_raises(Exception):
        evaluation([0.5])
    with ott.assert_raises(Exception):
        evaluation(ot.Sample([[0.5] * (dimension + 1)] * 2))
    with ott.assert_raises(Exception):
        f.gradient([0.5])
    with ott.assert_raises(Exception):
        f.hessian([0.5])
    with ott.assert_raises(Exception):
        gradient.gradient([0.5])
    with ott.assert_raises(Exception):
        hessian.hessian([0.5])
assert polyBasis.build(0) != polyBasis.build(1)

# __str__ branches: scalar, single and multiple non-constant factors
str(polyBasis.build(0).getEvaluation())
str(polyBasis.build(1).getEvaluation())
str(polyBasis.build(4).getEvaluation())

# Zero-product branch of gradient/hessian: build(1) vanishes at origin
ott.assert_almost_equal(polyBasis.build(1)([0.0, 0.0]), [0.0])
polyBasis.build(1).gradient([0.0, 0.0])
polyBasis.build(1).hessian([0.0, 0.0])

# Alternate ctors of ProductPolynomialEvaluation
coll = ot.PolynomialCollection(2)
coll[0] = ot.UniVariatePolynomial([1.0, 2.0])
coll[1] = ot.UniVariatePolynomial([0.0, 1.0])
evaluation = ot.ProductPolynomialEvaluation(coll)
copy = ot.ProductPolynomialEvaluation(evaluation)
assert evaluation == copy
assert not (evaluation != copy)
assert evaluation == evaluation
otherColl = ot.PolynomialCollection(2)
otherColl[0] = ot.UniVariatePolynomial([1.0, 3.0])
otherColl[1] = ot.UniVariatePolynomial([0.0, 1.0])
assert evaluation != ot.ProductPolynomialEvaluation(otherColl)
assert evaluation.getInputDimension() == 2
assert evaluation.getOutputDimension() == 1
assert "ProductPolynomialEvaluation" in repr(evaluation)
str(evaluation)
ott.assert_almost_equal(evaluation([0.5, 0.5]), [1.0])
ott.assert_almost_equal(
    evaluation([[0.5, 0.5], [1.0, 1.0]]), [[1.0], [3.0]]
)
with ott.assert_raises(Exception):
    evaluation([0.5])
with ott.assert_raises(Exception):
    evaluation(ot.Sample([[0.5] * 3] * 2))

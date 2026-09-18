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
    ot.TruncatedDistribution(ot.Normal(2.0, 1.5), 1.0, 4.0),
    ot.Uniform(),
]
basisFactory3 = ot.OrthogonalProductPolynomialFactory(distributionCollection)
print("basisFactory=")
print(basisFactory3)
print(basisFactory3.__repr_markdown__())
print(basisFactory3._repr_html_())

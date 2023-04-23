#! /usr/bin/env python

import openturns as ot

# Polynomial factories
factoryCollection = [ot.LaguerreFactory(2.5), ot.LegendreFactory(), ot.HermiteFactory()]
dim = len(factoryCollection)
basisFactory = ot.OrthogonalProductPolynomialFactory(factoryCollection)
print("basisFactory=")
print(basisFactory)
print(basisFactory._repr_markdown_())
print(basisFactory._repr_html_())

basis = ot.OrthogonalBasis(basisFactory)
print("basis=")
print(basis)

x = [0.5] * dim
for i in range(10):
    f = basis.build(i)
    print("i=", i, "f(X)=", f(x))

# Using multi-indices
enum = basis.getEnumerateFunction()
for i in range(10):
    indices = enum(i)
    f = basis.build(indices)
    print("indices=", indices, "f(X)=", f(x))

# Other factories
factoryCollection = [
    ot.OrthogonalUniVariatePolynomialFunctionFactory(ot.LaguerreFactory(2.5)),
    ot.HaarWaveletFactory(),
    ot.FourierSeriesFactory(),
]
dim = len(factoryCollection)
basisFactory = ot.OrthogonalProductFunctionFactory(factoryCollection)
basis = ot.OrthogonalBasis(basisFactory)
print("basis=", basis)
x = [0.5] * dim
for i in range(10):
    f = basis.build(i)
    print("i=", i, "f(X)=", f(x))

# Using multi-indices
enum = basis.getEnumerateFunction()
for i in range(10):
    indices = enum(i)
    f = basis.build(indices)
    print("indices=", indices, "f(X)=", f(x))

# Polynomial factories using a collection of distributions
distributionCollection = [ot.Normal(), ot.TruncatedDistribution(ot.Normal(2.0, 1.5), 1.0, 4.0), ot.Uniform()]
basisFactory = ot.OrthogonalProductPolynomialFactory(distributionCollection)
print("basisFactory=")
print(basisFactory)
print(basisFactory._repr_markdown_())
print(basisFactory._repr_html_())

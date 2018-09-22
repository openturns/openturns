#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

# Polynomial factories
factoryCollection = [ot.LaguerreFactory(
    2.5), ot.LegendreFactory(), ot.HermiteFactory()]
dim = len(factoryCollection)
basisFactory = ot.OrthogonalProductPolynomialFactory(factoryCollection)
basis = ot.OrthogonalBasis(basisFactory)
print('basis=', basis)
x = [0.5] * dim
for i in range(10):
    f = basis.build(i)
    print('i=', i, 'f(X)=', f(x))

# Using multi-indices
enum = basis.getEnumerateFunction()
for i in range(10):
    indices = enum(i)
    f = basis.build(indices)
    print('indices=', indices, 'f(X)=', f(x))

# Other factories
factoryCollection = [ot.OrthogonalUniVariatePolynomialFunctionFactory(
    ot.LaguerreFactory(2.5)),
    ot.HaarWaveletFactory(), ot.FourierSeriesFactory()]
dim = len(factoryCollection)
basisFactory = ot.OrthogonalProductFunctionFactory(factoryCollection)
basis = ot.OrthogonalBasis(basisFactory)
print('basis=', basis)
x = [0.5] * dim
for i in range(10):
    f = basis.build(i)
    print('i=', i, 'f(X)=', f(x))

# Using multi-indices
enum = basis.getEnumerateFunction()
for i in range(10):
    indices = enum(i)
    f = basis.build(indices)
    print('indices=', indices, 'f(X)=', f(x))

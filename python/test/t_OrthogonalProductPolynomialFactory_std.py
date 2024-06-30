#! /usr/bin/env python

import openturns as ot

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


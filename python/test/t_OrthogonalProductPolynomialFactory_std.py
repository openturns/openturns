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
    print(p.__repr_html__())

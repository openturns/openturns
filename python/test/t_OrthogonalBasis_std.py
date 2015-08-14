#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    dim = 3
    polynomCollection = PolynomialFamilyCollection(3)
    polynomCollection[0] = OrthogonalUniVariatePolynomialFamily(
        LaguerreFactory(2.5))
    polynomCollection[
        1] = OrthogonalUniVariatePolynomialFamily(LegendreFactory())
    polynomCollection[
        2] = OrthogonalUniVariatePolynomialFamily(HermiteFactory())
    basisFactory = OrthogonalProductPolynomialFactory(polynomCollection)
    basis = OrthogonalBasis(basisFactory)
    print("Basis=", basis)
    point = NumericalPoint(dim, 0.5)
    for i in range(10):
        f = NumericalMathFunction(basis.build(i))
        print("i=", i, " f(point)=", f(point))

except:
    import sys
    print("t_OrthogonalBasis_std.py", sys.exc_info()[0], sys.exc_info()[1])

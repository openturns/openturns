#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()


def check_orthonormality(factory, degree_max=6):
    m = degree_max + 1
    n, w = factory.getNodesAndWeights(m)
    for i in range(degree_max + 1):
        pI = factory.build(i)
        for j in range(i + 1):
            pJ = factory.build(j)
            val = sum(w[k] * pI(n[k]) * pJ(n[k]) for k in range(m))
            expected = 1.0 if i == j else 0.0
            ott.assert_almost_equal(val, expected, 1e-12)


def check_coefficients(factory, x_values):
    for i in range(10):
        p = factory.build(i)
        coeffs = p.getCoefficients()
        for x in x_values:
            y_coeff = sum(c * (x**j) for j, c in enumerate(coeffs))
            y_op = p(x)
            ott.assert_almost_equal(y_coeff, y_op)


# Standard factory
legendre = ot.LegendreFactory()
check_orthonormality(legendre)
check_coefficients(legendre, [0.0, 0.5, 1.0, -0.5, 2.0])

# Bounded factory
# Width 3 (not 2) so (bs-as)/(bi-ai)=2/3 vs inverse 3/2 is visible
a = 0.0
b = 3.0
legendre_bounded = ot.LegendreFactory(a, b)
check_orthonormality(legendre_bounded)
check_coefficients(legendre_bounded, [0.0, 1.0, 2.0, 3.0])
# Affine map from [0, 3] to standard [-1, 1]: a_=2/3, b_=-1
ott.assert_almost_equal(legendre_bounded.getA(), 2.0 / 3.0)
ott.assert_almost_equal(legendre_bounded.getB(), -1.0)

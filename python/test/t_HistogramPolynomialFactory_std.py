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


histogramPolynomial = ot.HistogramPolynomialFactory(
    -2.0, [1.0, 2.0, 1.5], [2.0, 5.0, 3.5]
)
check_orthonormality(histogramPolynomial)
check_coefficients(histogramPolynomial, [0.0, 0.5, 1.0, -0.5])

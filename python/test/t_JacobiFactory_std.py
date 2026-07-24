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


# Standard 2-arg factory
jacobi = ot.JacobiFactory(2.5, 3.5)
check_orthonormality(jacobi)
check_coefficients(jacobi, [0.0, 0.5, -0.5])

# Bounded 4-arg factory
alpha = 2.5
beta = 3.5
a = -1.0
b = 2.0
jacobi_bounded = ot.JacobiFactory(alpha, beta, a, b)
check_orthonormality(jacobi_bounded)
check_coefficients(jacobi_bounded, [a, (a + b) * 0.5, b])

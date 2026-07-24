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
chebychev = ot.ChebychevFactory()
check_orthonormality(chebychev)
check_coefficients(chebychev, [0.0, 0.5, -0.5])

# Bounded factory
a = 0.0
b = 2.0
chebychev_bounded = ot.ChebychevFactory(a, b)
check_orthonormality(chebychev_bounded)
check_coefficients(chebychev_bounded, [a, (a + b) * 0.5, b])

# Check roots/nodes/weights sizes
roots = chebychev_bounded.getRoots(10)
ott.assert_almost_equal(len(roots), 10)
nodes, weights = chebychev_bounded.getNodesAndWeights(10)
ott.assert_almost_equal(len(nodes), 10)
ott.assert_almost_equal(len(weights), 10)

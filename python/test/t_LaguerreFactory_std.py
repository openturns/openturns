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


# Standard 1-arg factory
laguerre = ot.LaguerreFactory(2.5)
check_orthonormality(laguerre)
check_coefficients(laguerre, [0.0, 0.5, 2.0])

# Non-standard 3-arg factory
# lam=2.0 (not 1.0) so lam vs 1/lam confusion is visible (2.0 vs 0.5)
k = 2.5
lam = 2.0
gam = -1.0
laguerre_nonstd = ot.LaguerreFactory(k, lam, gam)
check_orthonormality(laguerre_nonstd)
check_coefficients(laguerre_nonstd, [gam, gam + lam, gam + 2.0 * lam])
# Affine map is z = lam * (x - gam) since lam is a rate parameter
ott.assert_almost_equal(laguerre_nonstd.getA(), lam)
ott.assert_almost_equal(laguerre_nonstd.getB(), -lam * gam)
# Composition with the standard family: P_n^nonstd(x) = P_n^std(lam*(x-gam))
laguerre_std = ot.LaguerreFactory(k)
for n in range(5):
    p_std = laguerre_std.build(n)
    p_nstd = laguerre_nonstd.build(n)
    for x in [gam, gam + 0.5, gam + 2.0]:
        ott.assert_almost_equal(p_nstd(x), p_std(lam * (x - gam)))

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
hermite = ot.HermiteFactory()
check_orthonormality(hermite)
check_coefficients(hermite, [0.0, 0.5, 1.0, -0.5, 2.0])

# Non-standard factory
# sigma=2.0 (not 1.0) so 1/sigma vs sigma confusion is visible (0.5 vs 2.0)
mu = 1.0
sigma = 2.0
hermite_nonstd = ot.HermiteFactory(mu, sigma)
check_orthonormality(hermite_nonstd)
check_coefficients(hermite_nonstd, [mu - 2.0 * sigma, mu, mu + 2.0 * sigma])
# Affine map is z = (x - mu) / sigma
ott.assert_almost_equal(hermite_nonstd.getA(), 1.0 / sigma)
ott.assert_almost_equal(hermite_nonstd.getB(), -mu / sigma)
# Composition with the standard family: P_n^nonstd(x) = P_n^std((x-mu)/sigma)
hermite_std = ot.HermiteFactory()
for n in range(5):
    p_std = hermite_std.build(n)
    p_nstd = hermite_nonstd.build(n)
    for x in [mu - sigma, mu, mu + sigma]:
        ott.assert_almost_equal(p_nstd(x), p_std((x - mu) / sigma))

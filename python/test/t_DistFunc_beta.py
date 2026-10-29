#!/usr/bin/env python

import math

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

p1_values = [0.2, 1.4, 2.6, 3.8, 5.0]
p2_values = [0.2, 1.4, 2.6, 3.8, 5.0]
x_values = [0.1, 0.3, 0.5, 0.7, 0.9]
q_values = [0.1, 0.3, 0.5, 0.7, 0.9]

# pBeta: symmetry, complementary CDF and consistency with the Beta distribution
for p1 in p1_values:
    for p2 in p2_values:
        for x in x_values:
            value = ot.DistFunc.pBeta(p1, p2, x)
            assert 0.0 <= value <= 1.0
            ott.assert_almost_equal(
                ot.DistFunc.pBeta(p1, p2, x, True), 1.0 - value, 1e-12, 1e-15
            )
            ott.assert_almost_equal(
                value, 1.0 - ot.DistFunc.pBeta(p2, p1, 1.0 - x), 1e-12, 1e-15
            )

# pBeta: closed-form values
for x in x_values:
    ott.assert_almost_equal(ot.DistFunc.pBeta(1.0, 1.0, x), x, 1e-14, 0.0)
    ott.assert_almost_equal(
        ot.DistFunc.pBeta(1.0, 2.5, x), 1.0 - (1.0 - x) ** 2.5, 1e-14, 0.0
    )
    ott.assert_almost_equal(
        ot.DistFunc.pBeta(2.5, 1.0, x), x**2.5, 1e-14, 0.0
    )

# qBeta: inverse of pBeta, symmetry and tail flag
for p1 in p1_values:
    for p2 in p2_values:
        for q in q_values:
            value = ot.DistFunc.qBeta(p1, p2, q)
            assert 0.0 <= value <= 1.0
            ott.assert_almost_equal(
                ot.DistFunc.pBeta(p1, p2, value), q, 1e-10, 0.0
            )
            ott.assert_almost_equal(
                ot.DistFunc.qBeta(p1, p2, q, True),
                ot.DistFunc.qBeta(p1, p2, 1.0 - q),
                1e-12,
                0.0,
            )
            ott.assert_almost_equal(
                value,
                1.0 - ot.DistFunc.qBeta(p2, p1, 1.0 - q),
                1e-9,
                0.0,
            )

# qBeta: closed-form values
for q in q_values:
    ott.assert_almost_equal(ot.DistFunc.qBeta(1.0, 1.0, q), q, 1e-14, 0.0)
    ott.assert_almost_equal(
        ot.DistFunc.qBeta(1.0, 2.5, q), 1.0 - (1.0 - q) ** (1.0 / 2.5), 1e-13, 0.0
    )
    ott.assert_almost_equal(
        ot.DistFunc.qBeta(2.5, 1.0, q), q ** (1.0 / 2.5), 1e-13, 0.0
    )

# dBeta and logdBeta: consistency and closed-form values
for p1 in p1_values:
    for p2 in p2_values:
        for x in x_values:
            logpdf = ot.DistFunc.logdBeta(p1, p2, x)
            pdf = ot.DistFunc.dBeta(p1, p2, x)
            expected_logpdf = (
                (p1 - 1.0) * math.log(x)
                + (p2 - 1.0) * math.log1p(-x)
                - (math.lgamma(p1) + math.lgamma(p2) - math.lgamma(p1 + p2))
            )
            assert math.isfinite(logpdf)
            assert pdf > 0.0
            ott.assert_almost_equal(pdf, math.exp(logpdf), 1e-15, 0.0)
            ott.assert_almost_equal(logpdf, expected_logpdf, 1e-11, 0.0)
            ott.assert_almost_equal(
                logpdf, ot.DistFunc.logdBeta(p2, p1, 1.0 - x), 1e-13, 0.0
            )

ott.assert_almost_equal(ot.DistFunc.logdBeta(1.0, 1.0, 0.5), 0.0, 1e-15, 0.0)
ott.assert_almost_equal(ot.DistFunc.dBeta(1.0, 1.0, 0.5), 1.0, 1e-15, 0.0)

# Independent references: for integer shapes the CDF is a binomial sum,
# and B(2, 3) = 1 / 12
ott.assert_almost_equal(ot.DistFunc.pBeta(2.0, 3.0, 0.4), 0.5248, 1e-14, 0.0)
ott.assert_almost_equal(ot.DistFunc.pBeta(3.0, 2.0, 0.4), 0.1792, 1e-14, 0.0)
ott.assert_almost_equal(ot.DistFunc.dBeta(2.0, 3.0, 0.4), 1.728, 1e-14, 0.0)
ott.assert_almost_equal(
    ot.DistFunc.logdBeta(2.0, 3.0, 0.4), math.log(1.728), 1e-14, 0.0
)

# Boundary values and invalid parameters
for x in [0.0, 1.0, -0.5, 1.5]:
    assert ot.DistFunc.logdBeta(2.5, 3.5, x) == -math.inf
    assert ot.DistFunc.dBeta(2.5, 3.5, x) == 0.0
for p1, p2 in [(0.0, 1.0), (-1.0, 1.0), (1.0, 0.0), (1.0, -1.0)]:
    with ott.assert_raises(Exception):
        ot.DistFunc.logdBeta(p1, p2, 0.5)
    with ott.assert_raises(Exception):
        ot.DistFunc.dBeta(p1, p2, 0.5)

# rBeta: scalar and vector generations stay in the support
ot.RandomGenerator.SetSeed(0)
for p1 in p1_values:
    for p2 in p2_values:
        for _ in range(5):
            value = ot.DistFunc.rBeta(p1, p2)
            assert 0.0 <= value <= 1.0
        sample = ot.DistFunc.rBeta(p1, p2, 5000)
        assert len(sample) == 5000
        assert min(sample) >= 0.0
        assert max(sample) <= 1.0
        expected_mean = p1 / (p1 + p2)
        variance = p1 * p2 / ((p1 + p2) ** 2 * (p1 + p2 + 1.0))
        standard_error = math.sqrt(variance / len(sample))
        assert abs(sum(sample) / len(sample) - expected_mean) < 6.0 * standard_error

#!/usr/bin/env python

import math

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# rBinomial vector version: regression test for result[k] vs result[index] bug
ot.RandomGenerator.SetSeed(0)
# n*q <= 15 to force the small-case inversion branch
n, p, size = 10, 0.2, 5
res = ot.DistFunc.rBinomial(n, p, size)
ott.assert_almost_equal(res, [0, 3, 1, 4, 0])

# pPearsonCorrelation: regression test for abs(rho) bug
# Before fix, negative rho gave the same result as positive rho (wrong)
ott.assert_almost_equal(ot.DistFunc.pPearsonCorrelation(10, -0.5, False), 0.070556640625)
ott.assert_almost_equal(ot.DistFunc.pPearsonCorrelation(10, 0.0, False), 0.5)
ott.assert_almost_equal(ot.DistFunc.pPearsonCorrelation(10, 0.5, False), 0.929443359375)
# Tail version
ott.assert_almost_equal(ot.DistFunc.pPearsonCorrelation(10, -0.5, True), 0.929443359375)
ott.assert_almost_equal(ot.DistFunc.pPearsonCorrelation(10, 0.5, True), 0.070556640625)

# qDickeyFullerTrend / pDickeyFullerTrend consistency
ott.assert_almost_equal(ot.DistFunc.qDickeyFullerTrend(0.01, False), -3.96)
ott.assert_almost_equal(ot.DistFunc.qDickeyFullerTrend(0.05, False), -3.41)
ott.assert_almost_equal(ot.DistFunc.qDickeyFullerTrend(0.10, False), -3.13)
ott.assert_almost_equal(ot.DistFunc.pDickeyFullerTrend(-3.96, False), 0.01)
ott.assert_almost_equal(ot.DistFunc.pDickeyFullerTrend(-3.41, False), 0.05)
ott.assert_almost_equal(ot.DistFunc.pDickeyFullerTrend(-3.13, False), 0.10)

# rUniformTetrahedron
ot.RandomGenerator.SetSeed(0)
a = [0.0, 0.0, 0.0]
b = [1.0, 0.0, 0.0]
c = [0.0, 1.0, 0.0]
d = [0.0, 0.0, 1.0]
res = ot.DistFunc.rUniformTetrahedron(a, b, c, d)
ott.assert_almost_equal(res, [0.17851977638568556, 0.031017621207459226, 0.48665741512763006])

# rUniformTetrahedron dimension mismatch error message regression
# Before fix, it would print c.getDimension() instead of d.getDimension()
caught = False
try:
    ot.DistFunc.rUniformTetrahedron(a, b, c, [0.0, 0.0])
except TypeError as e:
    msg = str(e)
    if 'dimension=2' in msg and 'expected dimension=3' in msg:
        caught = True
assert caught, "Expected dimension mismatch error with correct point dimension"

# dBeta and logdBeta
for p1, p2, x in [
    (0.5, 0.5, 0.25),
    (1.0, 1.0, 0.5),
    (2.5, 3.5, 0.1),
    (2.5, 3.5, 0.5),
    (2.5, 3.5, 0.9),
    (0.2, 5.0, 0.7),
    (1000.0, 2000.0, 0.3),
]:
    expected_logpdf = (
        (p1 - 1.0) * math.log(x)
        + (p2 - 1.0) * math.log1p(-x)
        - (math.lgamma(p1) + math.lgamma(p2) - math.lgamma(p1 + p2))
    )
    ott.assert_almost_equal(
        ot.DistFunc.logdBeta(p1, p2, x), expected_logpdf, 1e-11, 0.0
    )
    ott.assert_almost_equal(
        ot.DistFunc.dBeta(p1, p2, x), math.exp(expected_logpdf), 1e-11, 0.0
    )
    ott.assert_almost_equal(
        ot.DistFunc.dBeta(p1, p2, x),
        math.exp(ot.DistFunc.logdBeta(p1, p2, x)),
        1e-15,
        0.0,
    )

# Symmetry and boundary conventions
for p1, p2, x in [(0.5, 2.5, 0.2), (2.5, 3.5, 0.75), (5.0, 0.2, 0.9)]:
    ott.assert_almost_equal(
        ot.DistFunc.logdBeta(p1, p2, x),
        ot.DistFunc.logdBeta(p2, p1, 1.0 - x),
        1e-13,
        0.0,
    )
for x in [0.0, 1.0, -0.5, 1.5]:
    assert ot.DistFunc.logdBeta(2.5, 3.5, x) == -math.inf
    assert ot.DistFunc.dBeta(2.5, 3.5, x) == 0.0
    assert math.exp(ot.DistFunc.logdBeta(2.5, 3.5, x)) == 0.0

# The log-density stays finite where the density underflows
logpdf = ot.DistFunc.logdBeta(2.5, 3.5, 1.0e-300)
assert math.isfinite(logpdf)
assert ot.DistFunc.dBeta(2.5, 3.5, 1.0e-300) == 0.0

# Invalid shape parameters
for p1, p2 in [(0.0, 1.0), (-1.0, 1.0), (1.0, 0.0), (1.0, -1.0)]:
    with ott.assert_raises(Exception):
        ot.DistFunc.logdBeta(p1, p2, 0.5)
    with ott.assert_raises(Exception):
        ot.DistFunc.dBeta(p1, p2, 0.5)

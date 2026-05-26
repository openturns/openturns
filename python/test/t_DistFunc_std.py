#!/usr/bin/env python

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

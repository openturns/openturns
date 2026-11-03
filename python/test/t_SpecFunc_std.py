#!/usr/bin/env python

import math

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

for lx in range(10):
    ott.assert_almost_equal(ot.SpecFunc.Log2(2**lx), lx, 1e-15, 0.0)

for lx in range(10):
    ott.assert_almost_equal(ot.SpecFunc.BitCount(2**lx - 1), lx, 0.0, 0.0)

for x, expected in [
    (0, 1),
    (1, 1),
    (2, 2),
    (3, 4),
    (4, 4),
    (10, 16),
    (100, 128),
    (1000, 1024),
]:
    ott.assert_almost_equal(ot.SpecFunc.NextPowerOfTwo(x), expected, 0.0, 0.0)

for x in [-2.0, -1.0, 0.0, 1.0, 2.0]:
    for n in [0, 1, 2]:
        ott.assert_almost_equal(ot.SpecFunc.IPow(x, n), x**n, 1e-15, 0.0)

for x, n, expected in [
    (-4.0, 1, -4.0),
    (4.0, 1, 4.0),
    (4.0, 2, 2.0),
    (9.0, 1, 9.0),
    (9.0, 2, 3.0),
]:
    ott.assert_almost_equal(ot.SpecFunc.IRoot(x, n), expected, 1e-15, 0.0)

ott.assert_almost_equal(
    ot.SpecFunc.RegularizedIncompleteBeta(2.0, 0.0, 0.95), 0.0, 1e-15, 0.0
)
ott.assert_almost_equal(
    ot.SpecFunc.RegularizedIncompleteBeta(0.0, 1.0, 0.95), 1.0, 1e-15, 0.0
)
ott.assert_almost_equal(
    ot.SpecFunc.RegularizedIncompleteBeta(2.0, 0.0, 0.95, True), 1.0, 1e-15, 0.0
)
ott.assert_almost_equal(
    ot.SpecFunc.RegularizedIncompleteBeta(0.0, 1.0, 0.95, True), 0.0, 1e-15, 0.0
)

x = [1.0, 2.0**53, -(2.0**53)]
s2 = ot.SpecFunc.AccurateSum(x)
assert s2 == 1.0, "accurate sum(x) not 1"

# Clip01
assert ot.SpecFunc.Clip01(0.2) == 0.2
assert ot.SpecFunc.Clip01(0.2, True) == 0.8
assert ot.SpecFunc.Clip01(-5.0) == 0.0
assert ot.SpecFunc.Clip01(5.0) == 1.0
assert ot.SpecFunc.Clip01(-5.0, True) == 1.0
assert ot.SpecFunc.Clip01(5.0, True) == 0.0

# Clip
assert ot.SpecFunc.Clip(1.7, 1.5, 2.3) == 1.7
assert ot.SpecFunc.Clip(1.2, 1.5, 2.3) == 1.5
assert ot.SpecFunc.Clip(2.6, 1.5, 2.3) == 2.3

# BesselInu/logBesselInu
# Reference values from Maple
# Check all the evaluation paths
params = [
    [1500.0, 2.5, 1495.4224506065839550, 2.8426174170203945360e649, 1e-15, None],
    [2000.0, 3.5, 1995.2776094874001304, 3.4518798080622279823e866, 1e-15, None],
    [35.0, 14.5, 29.303622290454120552, 5.3260036596760458083e12, 1e-15, 1e-14],
    [60.0, 16.5, 54.762680869475176692, 6.0691813672397311592e23, 1e-15, 1e-14],
    [275.0, 160.5, 225.57392772376926644, 9.2365986216876391890e97, 1e-15, 1e-13],
    [5.0, 300.5, -1142.3928958923650425, 7.3294122292332096791e-497, 1e-15, None],
    [85.0, 160.5, -45.398843277559942573, 1.9210243940397574031e-20, 1e-15, 1e-14],
    [5.0, 57.5, -125.63045577010892469, 2.7503394613839650378e-55, 1e-15, 1e-13],
    [40.0, 19.5, 32.520156685324717256, 1.3283869457280616997e14, 1e-15, 1e-14],
    [5.0, 21.5, -26.943567676609168226, 1.9886448747677890673e-12, 1e-15, 1e-14],
    [20.0, 12.5, 13.711695522233587559, 9.0139200731704084287e5, 1e-15, 1e-14],
    [5.0, 13.5, -11.067600244195792337, 1.5609977821969966950e-5, 1e-15, 1e-14],
    [15.0, 12.5, 7.6416656645837741856, 2083.2108588531338025, 1e-15, 1e-14],
]

for x, nu, logref, ref, epslog, eps in params:
    logy = ot.SpecFunc.LogBesselInu(x, nu)
    ott.assert_almost_equal(logy, logref, epslog, 0.0)
    if eps is not None:
        y = ot.SpecFunc.BesselInu(x, nu)
        ott.assert_almost_equal(y, ref, eps, 0.0)

# Test BesselInu with integer nu and negative x
y = ot.SpecFunc.BesselInu(-2.5, 2.0)
ref = 1.2764661478191642825
eps = 1e-15
ott.assert_almost_equal(y, ref, eps, 0.0)
y = ot.SpecFunc.BesselInu(-2.5, 3.0)
ref = -0.47437040877803558955
eps = 1e-15
ott.assert_almost_equal(y, ref, eps, 0.0)

# Test BesselJ/BesselJDerivative
# Reference values from scipy
for nu, x, ref, refprime in [
    [0.5, 3.0, 0.06500818287737592, -0.4668835179408628],
    [1.5, 10.0, 0.19798249275589247, -0.16696110966843367],
    [2.0, 5.0, 0.04656511627775229, -0.34620518410256607],
    [0.0, 1.0, 0.7651976865579666, -0.44005058574493355],
    [2.5, 0.5, 0.009236407819379731, 0.045519660528752726],
    [10.5, 25.0, -0.14462968429758666, 0.08015496250377363],
]:
    y = ot.SpecFunc.BesselJ(nu, x)
    ott.assert_almost_equal(y, ref, 1e-14, 0.0)
    yp = ot.SpecFunc.BesselJDerivative(nu, x)
    ott.assert_almost_equal(yp, refprime, 1e-14, 0.0)

# Integer reflections: J_2(-5) == J_2(5), J_{-3}(4) == -J_3(4)
y = ot.SpecFunc.BesselJ(2.0, -5.0)
ott.assert_almost_equal(y, 0.04656511627775229, 1e-14, 0.0)
y = ot.SpecFunc.BesselJ(-3.0, 4.0)
ott.assert_almost_equal(y, -0.43017147387562193, 1e-14, 0.0)
yp = ot.SpecFunc.BesselJDerivative(2.0, -5.0)
ott.assert_almost_equal(yp, 0.34620518410256607, 1e-14, 0.0)

# Non-finite inputs must throw instead of hanging in boost
for bad_nu, bad_x in [
    (float("inf"), 1.0),
    (float("nan"), 1.0),
    (0.0, float("inf")),
    (0.0, float("nan")),
]:
    with ott.assert_raises(TypeError):
        ot.SpecFunc.BesselJ(bad_nu, bad_x)
    with ott.assert_raises(TypeError):
        ot.SpecFunc.BesselJDerivative(bad_nu, bad_x)

# Noninteger order with negative argument has no real value and must throw;
# huge integer orders overflow UnsignedInteger and must throw
for bad_nu, bad_x in [
    (0.5, -1.0),
    (2.5, -0.5),
]:
    with ott.assert_raises(TypeError):
        ot.SpecFunc.BesselJ(bad_nu, bad_x)
    with ott.assert_raises(TypeError):
        ot.SpecFunc.BesselJDerivative(bad_nu, bad_x)
# LogHyperGeom_1_1
# Reference values from mpmath (50 digits)
params = [
    [0.5, 1.5, 1.0, 0.38025105262664983, 1e-12],
    [0.5, 1.5, 10.0, 7.0632454586326093, 1e-12],
    [0.5, 1.5, -7.5, -1.1283412648628479, 1e-12],
    [2.0, 3.0, 5.0, 3.8622487864048326, 1e-12],
    [2.0, 3.0, -30.0, -6.1092475827672663, 1e-12],
    [0.5, 1.5, 100.0, 94.706746732979597, 1e-10],
    [1.5, 0.5, 20.0, 23.713572066704308, 1e-10],
    [0.5, 1.5, -1000.0, -3.5746598771263137, 1e-10],
]
for p1, q1, x, logref, eps in params:
    logy = ot.SpecFunc.LogHyperGeom_1_1(p1, q1, x)
    ott.assert_almost_equal(logy, logref, eps, 0.0)

# Extended range: the logarithm stays finite far beyond the double range
# (log of the largest double is about 709.78)
logy = ot.SpecFunc.LogHyperGeom_1_1(0.5, 1.5, 1000.0)
assert math.isfinite(logy) and logy > 709.78
ott.assert_almost_equal(logy, 992.39959816700513, 1e-10, 0.0)

# Degenerate cases
ott.assert_almost_equal(ot.SpecFunc.LogHyperGeom_1_1(0.5, 1.5, 0.0), 0.0, 1e-15, 0.0)
ott.assert_almost_equal(ot.SpecFunc.LogHyperGeom_1_1(2.0, 2.0, 3.0), 3.0, 1e-15, 0.0)

# Nonpositive 1F1 has no real logarithm and must throw
with ott.assert_raises(TypeError):
    ot.SpecFunc.LogHyperGeom_1_1(-0.5, 1.0, 1000.0)

# Lauricella F_A
# In one dimension F_A reduces to the Gauss hypergeometric function 2F1
for x in [0.2, 0.5, 0.8]:
    value = ot.SpecFunc.LauricellaF_A(1.5, [2.0], [3.0], [x])
    expected = ot.SpecFunc.HyperGeom_2_1(1.5, 2.0, 3.0, x)
    ott.assert_almost_equal(value, expected, 1e-12, 0.0)
# A vanishing first parameter gives F_A = 1
ott.assert_almost_equal(
    ot.SpecFunc.LauricellaF_A(0.0, [2.0, 3.0], [4.0, 5.0], [0.2, 0.1]),
    1.0,
    1e-15,
    0.0,
)
# A vanishing variable reduces to the one-dimensional function
ott.assert_almost_equal(
    ot.SpecFunc.LauricellaF_A(1.5, [2.0, 3.0], [2.5, 4.0], [0.5, 0.0]),
    ot.SpecFunc.HyperGeom_2_1(1.5, 2.0, 2.5, 0.5),
    1e-12,
    0.0,
)
# Reference values from mpmath (50 digits)
lauricella_params = [
    [0.5, [1.5, 2.5], [2.0, 3.0], [0.2, 0.1], 1.1440685953643620],
    [-2.5, [1.5, 2.5], [2.0, 3.0], [0.3, -0.2], 0.87335183445907323],
    [2.5, [0.5, 1.5], [3.0, 4.0], [0.1, 0.2], 1.2873765836948652],
]
for a, b, c, x, ref in lauricella_params:
    value = ot.SpecFunc.LauricellaF_A(a, b, c, x)
    ott.assert_almost_equal(value, ref, 1e-12, 0.0)

# LogLauricellaF_A returns the logarithm, the sign and a cancellation guess
available, logValue, cancellation, sign = ot.SpecFunc.LogLauricellaF_A(
    1.5, [2.0], [3.0], [0.5]
)
assert available
ott.assert_almost_equal(
    sign * math.exp(logValue),
    ot.SpecFunc.LauricellaF_A(1.5, [2.0], [3.0], [0.5]),
    1e-12,
    0.0,
)
assert cancellation > 0.0
# A negative value is reported through the sign
available, logValue, cancellation, sign = ot.SpecFunc.LogLauricellaF_A(
    -1.0, [3.0], [1.0], [0.5]
)
assert available
ott.assert_almost_equal(sign, -1.0, 0.0, 0.0)
ott.assert_almost_equal(sign * math.exp(logValue), -0.5, 1e-15, 0.0)
assert cancellation > 1.0
# Rejected domains are reported through the returned flag
for b, c, x in [
    ([2.0], [0.0], [0.2]),
    ([2.0], [3.0], [float("nan")]),
    ([2.0, 3.0], [4.0, 5.0], [0.6, 0.5]),
]:
    assert not ot.SpecFunc.LogLauricellaF_A(1.5, b, c, x)[0]
with ott.assert_raises(TypeError):
    ot.SpecFunc.LogLauricellaF_A(1.5, [2.0, 3.0], [4.0], [0.2, 0.1])

# The series remains accurate close to its convergence radius, where the
# terms decay slowly
ott.assert_almost_equal(
    ot.SpecFunc.LauricellaF_A(1.5, [2.0], [3.0], [0.87]),
    ot.SpecFunc.HyperGeom_2_1(1.5, 2.0, 3.0, 0.87),
    1e-12,
    0.0,
)
# Closer to the radius, where hundreds of terms are needed, the result
# remains accurate.
ott.assert_almost_equal(
    ot.SpecFunc.LauricellaF_A(-0.5, [2.0], [3.0], [0.975]),
    ot.SpecFunc.HyperGeom_2_1(-0.5, 2.0, 3.0, 0.975),
    1e-11,
    0.0,
)

# Mismatched dimensions are a usage error
with ott.assert_raises(TypeError):
    ot.SpecFunc.LauricellaF_A(1.5, [2.0, 3.0], [4.0], [0.2, 0.1])

# Degenerate inputs and rejected domains return NaN
assert ot.SpecFunc.LauricellaF_A(1.5, [], [], []) == 1.0
for b, c, x in [
    ([2.0], [0.0], [0.2]),
    ([2.0], [3.0], [float("nan")]),
    ([2.0], [3.0], [float("inf")]),
    ([2.0, 3.0], [4.0, 5.0], [0.6, 0.5]),
]:
    assert math.isnan(ot.SpecFunc.LauricellaF_A(1.5, b, c, x))

# A tolerance outside (0, 1), including NaN, is rejected
lauricellaTolerance = ot.ResourceMap.GetAsScalar("SpecFunc-LauricellaTolerance")
try:
    for tolerance in [0.0, -1.0, 1.0, 2.0, float("nan")]:
        ot.ResourceMap.SetAsScalar("SpecFunc-LauricellaTolerance", tolerance)
        with ott.assert_raises(Exception):
            ot.SpecFunc.LauricellaF_A(1.5, [2.0], [3.0], [0.5])
finally:
    ot.ResourceMap.SetAsScalar(
        "SpecFunc-LauricellaTolerance", lauricellaTolerance
    )

# A series stopped at its degree limit without convergence must not be
# reported as a valid value: the caller has to use its fallback.
lauricellaMaximumDegree = ot.ResourceMap.GetAsUnsignedInteger(
    "SpecFunc-LauricellaMaximumDegree"
)
try:
    ot.ResourceMap.SetAsUnsignedInteger("SpecFunc-LauricellaMaximumDegree", 64)
    lauricella_args = (0.5, [1.0] * 8, [2.0] * 8, [0.99] + [0.0] * 7)
    assert math.isnan(ot.SpecFunc.LauricellaF_A(*lauricella_args))
    assert not ot.SpecFunc.LogLauricellaF_A(*lauricella_args)[0]
finally:
    ot.ResourceMap.SetAsUnsignedInteger(
        "SpecFunc-LauricellaMaximumDegree", lauricellaMaximumDegree
    )

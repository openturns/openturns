#!/usr/bin/env python

import openturns as ot
import openturns.testing as ott

for lx in range(10):
    x = 2**lx
    print("x=", x, "log2=", ot.SpecFunc.Log2(x))

for lx in range(10):
    x = 2**lx - 1
    print("x=", x, "bits=", ot.SpecFunc.BitCount(x))

for x in [0, 1, 2, 3, 4, 10, 100, 1000]:
    print("x=", x, "nextpow2=", ot.SpecFunc.NextPowerOfTwo(x))

for x in [-2.0, -1.0, 0.0, 1.0, 2.0]:
    for n in [0, 1, 2]:
        print("x=", x, "n=", n, "ipow=", ot.SpecFunc.IPow(x, n))

for x in [-4.0, 4.0, 9.0]:
    for n in [1, 2]:
        if x < 0.0 and n % 2 == 0:
            continue
        print("x=", x, "n=", n, "iroot=", ot.SpecFunc.IRoot(x, n))

print("ibeta(2.0, 0.0, 0.95)=", ot.SpecFunc.RegularizedIncompleteBeta(2.0, 0.0, 0.95))
print("ibeta(0.0, 1.0, 0.95)=", ot.SpecFunc.RegularizedIncompleteBeta(0.0, 1.0, 0.95))
print(
    "ibeta(2.0, 0.0, 0.95, tail)=",
    ot.SpecFunc.RegularizedIncompleteBeta(2.0, 0.0, 0.95, True),
)
print(
    "ibeta(0.0, 1.0, 0.95, tail)=",
    ot.SpecFunc.RegularizedIncompleteBeta(0.0, 1.0, 0.95, True),
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
params = [[1500.0, 2.5, 1495.4224506065839550, 2.8426174170203945360e649, 1e-15, None],
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
          [15.0, 12.5, 7.6416656645837741856, 2083.2108588531338025, 1e-15, 1e-14]]

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

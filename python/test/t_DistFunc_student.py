#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# pStudent
nuMin = 0.2
nuMax = 5.0
n1 = 5
xMin = 0.1
xMax = 0.9
nX = 10
grid = [0.0] * nX
for i1 in range(n1):
    nu = nuMin + (nuMax - nuMin) * i1 / (n1 - 1)
    for iX in range(nX):
        x = xMin + (xMax - xMin) * iX / (nX - 1)
        grid[iX] = x
        print(
            "pStudent(",
            nu,
            ",  %.12g" % x,
            ")=%.6g" % ot.DistFunc.pStudent(nu, x),
            ", complementary=%.6g" % ot.DistFunc.pStudent(nu, x, True),
        )
    print("pStudent(", grid, ")=", ot.DistFunc.pStudent(nu, grid))

# check for nans
for x in [-1e300, -1e200, -1e100, 1e10, -10.0, -0.1, 0.0, 0.1, 10.0, 1e10, 1e100, 1e200, 1e300]:
    for nu in [2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0, 5.5, 6.0, 6.5, 7.0, 7.5]:
        for tail in [False, True]:
            p = ot.DistFunc.pStudent(nu, x, tail)
            assert ot.SpecFunc.IsNormal(p), "pStudent returns nan"

# qStudent
nuMin = 0.2
nuMax = 5.0
n1 = 5
qMin = 0.1
qMax = 0.9
nQ = 10
grid = [0.0] * nQ
for i1 in range(n1):
    nu = nuMin + (nuMax - nuMin) * i1 / (n1 - 1)
    for iQ in range(nQ):
        q = qMin + (qMax - qMin) * iQ / (nQ - 1)
        grid[iQ] = q
        print(
            "qStudent(",
            nu,
            ",  %.12g" % q,
            ")=%.6g" % ot.DistFunc.qStudent(nu, q),
            ", complementary=%.6g" % ot.DistFunc.qStudent(nu, q, True),
        )
print("qStudent(", grid, ")=", ot.DistFunc.qStudent(nu, grid))
# rStudent
nuMin = 0.2
nuMax = 5.0
n1 = 5
nR = 10
for i1 in range(n1):
    nu = nuMin + (nuMax - nuMin) * i1 / (n1 - 1)
    for iR in range(nR):
        print("rStudent(", nu, ")=%.6g" % ot.DistFunc.rStudent(nu))

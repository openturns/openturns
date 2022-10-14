#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


# pNormal
xMin = 0.1
xMax = 0.9
nX = 10
grid = [0.0] * nX
for iX in range(nX):
    x = xMin + (xMax - xMin) * iX / (nX - 1)
    grid[iX] = x
    print(
        "pNormal( %.12g" % x,
        ")=%.6g" % ot.DistFunc.pNormal(x),
        ", complementary=%.6g" % ot.DistFunc.pNormal(x, True),
    )
print("pNormal(", grid, "=", ot.DistFunc.pNormal(grid))
# qNormal
qMin = 0.1
qMax = 0.9
nQ = 10
grid = [0.0] * nQ
for iQ in range(nQ):
    q = qMin + (qMax - qMin) * iQ / (nQ - 1)
    grid[iQ] = q
    print(
        "qNormal( %.12g" % q,
        ")=%.6g" % ot.DistFunc.qNormal(q),
        ", complementary=%.6g" % ot.DistFunc.qNormal(q, True),
    )
print("qNormal(", grid, ")=", ot.DistFunc.qNormal(grid))
# rNormal
nR = 10
for iR in range(nR):
    print("rNormal()=%.6g" % ot.DistFunc.rNormal())

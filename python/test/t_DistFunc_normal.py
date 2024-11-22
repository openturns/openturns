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
    print(f"pNormal({x:.12g})={ot.DistFunc.pNormal(x):.6g}, complementary={ot.DistFunc.pNormal(x, True):.6g}")
print("pNormal(", grid, "=", ot.DistFunc.pNormal(grid))
# qNormal
qMin = 0.0
qMax = 1.0
nQ = 11
grid = [0.0] * nQ
for iQ in range(nQ):
    q = qMin + (qMax - qMin) * iQ / (nQ - 1)
    grid[iQ] = q
    print(f"qNormal({q:.12g})={ot.DistFunc.qNormal(q):.6g}, complementary={ot.DistFunc.qNormal(q, True):.6g}")
print("qNormal(", grid, ")=", ot.DistFunc.qNormal(grid))
# rNormal
nR = 10
for iR in range(nR):
    print(f"rNormal()={ot.DistFunc.rNormal():.6g}")

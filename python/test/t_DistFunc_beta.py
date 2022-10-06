#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# Beta related functions
# pBeta
p1Min = 0.2
p1Max = 5.0
n1 = 5
p2Min = 0.2
p2Max = 5.0
n2 = 5
xMin = 0.1
xMax = 0.9
nX = 5
for i1 in range(n1):
    p1 = p1Min + (p1Max - p1Min) * i1 / (n1 - 1)
    for i2 in range(n2):
        p2 = p2Min + (p2Max - p2Min) * i2 / (n2 - 1)
        for iX in range(nX):
            x = xMin + (xMax - xMin) * iX / (nX - 1)
            print("pBeta(", p1, ", ", p2, ",  %.12g" % x, ")=%.6g" % ot.DistFunc.pBeta(
                p1, p2, x), ", complementary=%.6g" % ot.DistFunc.pBeta(p1, p2, x, True))
# qBeta
p1Min = 0.2
p1Max = 5.0
n1 = 5
p2Min = 0.2
p2Max = 5.0
n2 = 5
qMin = 0.1
qMax = 0.9
nQ = 5
for i1 in range(n1):
    p1 = p1Min + (p1Max - p1Min) * i1 / (n1 - 1)
    for i2 in range(n2):
        p2 = p2Min + (p2Max - p2Min) * i2 / (n2 - 1)
        for iQ in range(nQ):
            q = qMin + (qMax - qMin) * iQ / (nQ - 1)
            print("qBeta(", p1, ", ", p2, ",  %.12g" % q, ")=%.6g" % ot.DistFunc.qBeta(
                p1, p2, q), ", complementary=%.6g" % ot.DistFunc.qBeta(p1, p2, q, True))
# rBeta
p1Min = 0.2
p1Max = 5.0
n1 = 5
p2Min = 0.2
p2Max = 5.0
n2 = 5
nR = 5
for i1 in range(n1):
    p1 = p1Min + (p1Max - p1Min) * i1 / (n1 - 1)
    for i2 in range(n2):
        p2 = p2Min + (p2Max - p2Min) * i2 / (n2 - 1)
        for iR in range(nR):
            print("rBeta(", p1, ", ", p2, ")=%.6g" %
                  ot.DistFunc.rBeta(p1, p2))



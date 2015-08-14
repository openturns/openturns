#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    # pNormal
    xMin = 0.1
    xMax = 0.9
    nX = 10
    for iX in range(nX):
        x = xMin + (xMax - xMin) * iX / (nX - 1)
        print("pNormal( %.12g" % x, ")=%.6g" % DistFunc.pNormal(
            x), ", complementary=%.6g" % DistFunc.pNormal(x, True))
    # qNormal
    qMin = 0.1
    qMax = 0.9
    nQ = 10
    for iQ in range(nQ):
        q = qMin + (qMax - qMin) * iQ / (nQ - 1)
        print("qNormal( %.12g" % q, ")=%.6g" % DistFunc.qNormal(
            q), ", complementary=%.6g" % DistFunc.qNormal(q, True))
    # rNormal
    nR = 10
    for iR in range(nR):
        print("rNormal()=%.6g" % DistFunc.rNormal())
except:
    import sys
    print("t_DistFunc_normal.py", sys.exc_info()[0], sys.exc_info()[1])

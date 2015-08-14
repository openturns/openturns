#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    # Gamma related functions
    # pGamma
    kMin = 0.2
    kMax = 5.0
    nK = 5
    xMin = 0.1
    xMax = 0.9
    nX = 5
    for i1 in range(nK):
        k = kMin + (kMax - kMin) * i1 / (nK - 1)
        for iX in range(nX):
            x = xMin + (xMax - xMin) * iX / (nX - 1)
            print("pGamma(", k, ",  %.12g" % x, ")=%.6g" % DistFunc.pGamma(
                k, x), ", complementary=%.6g" % DistFunc.pGamma(k, x, True))
    # qGamma
    kMin = 0.2
    kMax = 5.0
    nK = 5
    qMin = 0.1
    qMax = 0.9
    nQ = 5
    for i1 in range(nK):
        k = kMin + (kMax - kMin) * i1 / (nK - 1)
        for iQ in range(nQ):
            q = qMin + (qMax - qMin) * iQ / (nQ - 1)
            print("qGamma(", k, ",  %.12g" % q, ")=%.6g" % DistFunc.qGamma(
                k, q), ", complementary=%.6g" % DistFunc.qGamma(k, q, True))
    # rGamma
    kMin = 0.2
    kMax = 5.0
    nK = 5
    nR = 5
    for i1 in range(nK):
        k = kMin + (kMax - kMin) * i1 / (nK - 1)
        for iR in range(nR):
            print("rGamma(", k, ")=%.6g" % DistFunc.rGamma(k))

except:
    import sys
    print("t_DistFunc_gamma.py", sys.exc_info()[0], sys.exc_info()[1])

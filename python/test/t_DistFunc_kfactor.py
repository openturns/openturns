#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    nMin = 2
    nMax = 4
    pMin = 0.1
    pMax = 0.9
    nP = 3
    alphaMin = 0.1
    alphaMax = 0.9
    nAlpha = 3
    for n in range(nMin, nMax):
        for iP in range(nP):
            p = pMin + (pMax - pMin) * iP / (nP + 1.0)
            for iAlpha in range(nAlpha):
                alpha = alphaMin + (alphaMax - alphaMin) * iAlpha / (nAlpha + 1.0)
                print("kFactor(", 2 * n, ", ", p, ", ", alpha, ")=%.5f" % DistFunc.kFactor(2 * n, p, alpha))
    mMin = 1
    mMax = 3
    for n in range(nMin, nMax):
        for m in range(mMin, mMax):
            for iP in range(nP):
                p = pMin + (pMax - pMin) * iP / (nP + 1.0)
                for iAlpha in range(nAlpha):
                    alpha = alphaMin + (alphaMax - alphaMin) * iAlpha / (nAlpha + 1.0)
                    print("kFactorPooled(", 2 * n, ", ", m, ", ", p, ", ", alpha, ")=%.5f" % DistFunc.kFactor(2 * n, p, alpha))
except:
    import sys
    print("t_DistFunc_kfactor.py", sys.exc_info()[0], sys.exc_info()[1])

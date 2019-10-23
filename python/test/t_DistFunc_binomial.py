#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    # Binomial related functions
    # dBinomial
    pMin = 0.1
    pMax = 0.9
    nMin = 2
    nMax = 12
    n1 = 5
    for i1 in range(n1):
        p = pMin + i1 * (pMax - pMin) / (n1 - 1.0)
        for n in range(nMin, nMax + 1):
            for k in range(n + 1):
                print("dBinomial(", n, ", %.3g" % p, ", ", k, ")=%.2g" %
                      DistFunc.dBinomial(n, p, k))
                # logdBinomial
    for i1 in range(n1):
        p = pMin + i1 * (pMax - pMin) / (n1 - 1.0)
        for n in range(nMin, nMax + 1):
            for k in range(n + 1):
                print("logdBinomial(", n, ", %.3g" % p, ", ", k, ")=%.2g" %
                      DistFunc.logdBinomial(n, p, k))
                # rBinomial
    nR = 5
    for i1 in range(n1):
        p = pMin + i1 * (pMax - pMin) / (n1 - 1.0)
        for n in range(nMin, nMax + 1):
            for iR in range(nR):
                print("rBinomial()=", DistFunc.rBinomial(n, p))
except:
    import sys
    print("t_DistFunc_binomial.py", sys.exc_info()[0], sys.exc_info()[1])

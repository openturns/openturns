#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    # Kolmogorov related functions
    nValues = [2, 5, 10, 20, 50, 100, 200, 500,
               1000, 2000, 5000, 10000, 20000, 100000, 1000000]
    xValues = [0.0001, 0.0002, 0.0005, 0.001, 0.002, 0.005, 0.01, 0.02,
               0.05, 0.1, 0.2, 0.5, 1.0, 1.2, 1.5, 2.0, 5.0, 10.0, 20.0, 50.0]
    for i in range(len(nValues)):
        n = nValues[i]
        for j in range(len(xValues)):
            x = xValues[j]
            cdf = DistFunc.pKolmogorov(n, x)
            if cdf < 1e-16:
                cdf = 0.0
            ccdf = DistFunc.pKolmogorov(n, x, True)
            if ccdf < 1e-16:
                ccdf = 0.0
            print("pKolmogorov(", n, ",  %.12g" % x, ")=%.6g" %
                  cdf, ", complementary=%.6g" % ccdf)

except:
    import sys
    print("t_DistFunc_kolmogorov.py", sys.exc_info()[0], sys.exc_info()[1])

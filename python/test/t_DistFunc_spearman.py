#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    # Spearman related functions
    lSize = [5, 10, 25, 50, 1000]
    for size in lSize:
        for ties in [True, False]:
            for tail in [True, False]:
                for iRho in range(-10, 11):
                    rho = 0.1 * iRho
                    print("size=", size, " ties=", ties, "tail=", tail, " rho=%.6g" %
                          rho, " p=%.6g" % DistFunc.pSpearmanCorrelation(size, rho, tail, ties))

except:
    import sys
    print("t_DistFunc_spearman.py", sys.exc_info()[0], sys.exc_info()[1])

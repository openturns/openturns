#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    coll = [GumbelCopula(3.0), ClaytonCopula(3.0), FrankCopula(3.0)]
    size = 1000
    for i in range(len(coll)):
        ref_copula = coll[i]
        print("Reference copula", str(ref_copula))
        sample = ref_copula.getSample(size)
        est_copula = BernsteinCopulaFactory().build(sample)
        max_error = 0.0
        for m in range(11):
            for n in range(11):
                point = [0.1 * m, 0.1 * n]
                max_error = max(max_error,
                                abs(ref_copula.computeCDF(point) - est_copula.computeCDF(point)))
        print("Max. error=%.5f" % max_error)

except:
    import sys
    print("t_BernsteinCopulaFactory_std.py",
          sys.exc_info()[0], sys.exc_info()[1])

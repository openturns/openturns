#! /usr/bin/env python

from __future__ import print_function
from openturns import *
from math import *

TESTPREAMBLE()

try:
    def clean(inPoint):
        dim = inPoint.getDimension()
        for i in range(dim):
            if abs(inPoint[i]) < 1.e-10:
                inPoint[i] = 0.0
        return inPoint

    iMax = 5
    distribution = Triangular(-1.0, 0.3, 1.0)
    algo = GramSchmidtAlgorithm(distribution)
    print("algo=", repr(algo))
    for i in range(iMax):
        print(distribution.getClassName() + " polynomial(", i, ")=",
              clean(algo.getRecurrenceCoefficients(i)))
    algo.setReferenceFamily(
        OrthogonalUniVariatePolynomialFamily(LegendreFactory()))
    print("Reference family=", algo.getReferenceFamily())

except:
    import sys
    print("t_GramSchmidtAlgorithm_std.py",
          sys.exc_info()[0], sys.exc_info()[1])

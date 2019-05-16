#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    sqn = SquaredNormal()
    print("Distribution ", repr(sqn))
    print("Distribution ", sqn)
    rng = sqn.getRange()
    print("range = ", rng)
    cdf = sqn.computeCDF(1.0)
    print("cdf = %.6f" % cdf)
    pdf = sqn.computePDF(1.0)
    print("pdf = %.6f" % pdf)
    sample = sqn.getSample(5)
    print("sample(5) = ", sample)
    mean = sqn.getMean()
    print("mean = ", mean)
    covariance = sqn.getCovariance()
    print("covariance = ", covariance)
    
except:
    import sys
    print("t_SquaredNormal_std.py", sys.exc_info()[0], sys.exc_info()[1])

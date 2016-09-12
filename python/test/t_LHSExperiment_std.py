#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    distribution = Normal(4)
    size = 10
    myPlane = LHSExperiment(distribution, size)
    print("myPlane = ", myPlane)
    sample, weights = myPlane.generateWithWeights()
    print("sample = ", repr(sample))
    print("weights = ", repr(weights))
    print("sample2 = ", myPlane.generate())
    myPlane = LHSExperiment(distribution, size, True, True)
    print("sample  = ", myPlane.generate())
    print("sample2 = ", myPlane.generate())
    myPlane = LHSExperiment(distribution, size, False, False)
    print("sample  = ", myPlane.generate())
    print("sample2 = ", myPlane.generate())
    myPlane = LHSExperiment(distribution, size, True, False)
    print("sample  = ", myPlane.generate())
    print("sample2 = ", myPlane.generate())
    myPlane = LHSExperiment(size)
    print("sample  = ", myPlane.generate())
    print("sample2 = ", myPlane.generate())
except:
    import sys
    print("t_LHSExperiment_std.py", sys.exc_info()[0], sys.exc_info()[1])

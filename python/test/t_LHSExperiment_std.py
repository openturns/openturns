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
except:
    import sys
    print("t_LHSExperiment_std.py", sys.exc_info()[0], sys.exc_info()[1])

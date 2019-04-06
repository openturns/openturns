#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    distribution = Normal(4)
    size = 10
    myExperiment = LHSExperiment(distribution, size)
    print("myExperiment = ", myExperiment)
    sample, weights = myExperiment.generateWithWeights()
    print("sample = ", repr(sample))
    print("weights = ", repr(weights))
    print("sample2 = ", myExperiment.generate())
    myExperiment = LHSExperiment(distribution, size, True, True)
    print("sample  = ", myExperiment.generate())
    print("sample2 = ", myExperiment.generate())
    myExperiment = LHSExperiment(distribution, size, False, False)
    print("sample  = ", myExperiment.generate())
    print("sample2 = ", myExperiment.generate())
    myExperiment = LHSExperiment(distribution, size, True, False)
    print("sample  = ", myExperiment.generate())
    print("sample2 = ", myExperiment.generate())
    myExperiment = LHSExperiment(size)
    print("sample  = ", myExperiment.generate())
    print("sample2 = ", myExperiment.generate())
except:
    import sys
    print("t_LHSExperiment_std.py", sys.exc_info()[0], sys.exc_info()[1])

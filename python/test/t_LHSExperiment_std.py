#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    distribution = Normal(4)
    size = 10
    experiment = LHSExperiment(distribution, size)
    print("experiment = ", experiment)
    sample, weights = experiment.generateWithWeights()
    print("sample = ", repr(sample))
    print("weights = ", repr(weights))
    print("sample2 = ", experiment.generate())
    experiment = LHSExperiment(distribution, size, True, True)
    print("sample  = ", experiment.generate())
    print("sample2 = ", experiment.generate())
    experiment = LHSExperiment(distribution, size, False, False)
    print("sample  = ", experiment.generate())
    print("sample2 = ", experiment.generate())
    experiment = LHSExperiment(distribution, size, True, False)
    print("sample  = ", experiment.generate())
    print("sample2 = ", experiment.generate())
    experiment = LHSExperiment(size)
    print("sample  = ", experiment.generate())
    print("sample2 = ", experiment.generate())
except:
    import sys
    print("t_LHSExperiment_std.py", sys.exc_info()[0], sys.exc_info()[1])

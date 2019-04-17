#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    distribution = Normal(4)
    size = 10
    experiment = MonteCarloExperiment(distribution, size)
    print("experiment = ", experiment)
    sample, weights = experiment.generateWithWeights()
    print("sample = ", repr(sample))
    print("weights = ", repr(weights))
except:
    import sys
    print("t_MonteCarloExperiment_std.py",
          sys.exc_info()[0], sys.exc_info()[1])

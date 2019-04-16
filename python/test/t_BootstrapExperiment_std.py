#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    size = 5
    dim = 6
    refSample = Sample(size, dim)
    for i in range(size):
        p = Point(dim)
        for j in range(dim):
            p[j] = i + j
        refSample[i] = p
    print("ref. sample=", repr(refSample))
    experiment = BootstrapExperiment(refSample)
    print("experiment = ", experiment)
    sample, weights = experiment.generateWithWeights()
    print("sample = ", repr(sample))
    print("weights = ", repr(weights))
    print("selection=", BootstrapExperiment.GenerateSelection(size, size))
except:
    import sys
    print("t_BootstrapExperiment_std.py", sys.exc_info()[0], sys.exc_info()[1])

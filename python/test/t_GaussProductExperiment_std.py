#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    distribution = ComposedDistribution(
        DistributionCollection([Exponential(), Triangular(-1.0, -0.5, 1.0)]))
    marginalDegrees = Indices([3, 6])
    # Test the constructor based on marginal degrees
    print("experiment1=", GaussProductExperiment(marginalDegrees))
    # Test the constructor based on distribution
    print("experiment2=", GaussProductExperiment(distribution))
    # Test the constructor based on marginal degrees and distribution
    experiment = GaussProductExperiment(
        Distribution(distribution), marginalDegrees)
    print("experiment = ", experiment)
    sample, weights = experiment.generateWithWeights()
    print("sample = ", repr(sample))
    print("weights = ", repr(weights))
except:
    import sys
    print("t_GaussProductExperiment_std.py",
          sys.exc_info()[0], sys.exc_info()[1])

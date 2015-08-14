#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    distribution = Normal(4)
    distribution.setMean(NumericalPoint(4, 5.0))
    size = 10
    myPlane = LowDiscrepancyExperiment(HaltonSequence(), distribution, size)
    print("myPlane = ", myPlane)
    sample, weights = myPlane.generateWithWeights()
    print("sample = ", repr(sample))
    print("weights = ", repr(weights))
except:
    import sys
    print("t_LowDiscrepancyExperiment_std.py",
          sys.exc_info()[0], sys.exc_info()[1])

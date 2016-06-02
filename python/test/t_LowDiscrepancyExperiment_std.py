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
    print("sample  = ", repr(sample))
    print("weights = ", repr(weights))
    print("sample2 = ", repr(myPlane.generate()))
    myPlane.setDistribution(distribution)
    print("sample3 = ", repr(myPlane.generate()))
    myPlane = LowDiscrepancyExperiment(HaltonSequence(), distribution, size, False)
    print("sample  = ", repr(myPlane.generate()))
    print("sample2 = ", repr(myPlane.generate()))
    myPlane.setDistribution(distribution)
    print("sample3 = ", repr(myPlane.generate()))
    myPlane = LowDiscrepancyExperiment(HaltonSequence(2), size)
    print("sample4 = ", repr(myPlane.generate()))
except:
    import sys
    print("t_LowDiscrepancyExperiment_std.py",
          sys.exc_info()[0], sys.exc_info()[1])

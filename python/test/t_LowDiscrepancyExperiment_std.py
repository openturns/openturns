#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    distribution = Normal(4)
    distribution.setMean([5.0]*4)
    size = 16
    myPlane = LowDiscrepancyExperiment(HaltonSequence(), distribution, size)
    print("myPlane = ", myPlane)
    # Test sampling with weights
    sample, weights = myPlane.generateWithWeights()
    print("sample  = ", sample)
    print("weights = ", weights)
    # Test sampling with reinitialization each time the distribution is
    # set (default behaviour)
    # sample 2 != sample
    print("sample2 = ", myPlane.generate())
    myPlane.setDistribution(distribution)
    # sample 3 == sample
    print("sample3 = ", myPlane.generate())

    # Test sampling without reinitialization excepted when distribution
    # dimension changes
    myPlane = LowDiscrepancyExperiment(
        HaltonSequence(), distribution, size, False)
    print("sample  = ", myPlane.generate())
    # sample 2 != sample
    print("sample2 = ", myPlane.generate())
    myPlane.setDistribution(distribution)
    # sample 3 != sample && sample 3 != sample 2
    print("sample3 = ", myPlane.generate())
    # Test dimension change
    myPlane.setDistribution(Normal())
    print("sample = ", myPlane.generate())

    # Test constructor with no distribution and dimension>1
    myPlane = LowDiscrepancyExperiment(HaltonSequence(2), size)
    print("sample = ", myPlane.generate())
    # Test with dependent marginals
    R = CorrelationMatrix(4)
    for i in range(1, 4):
        R[i - 1, i] = 0.5
    distribution.setCorrelation(R)
    myPlane = LowDiscrepancyExperiment(
        HaltonSequence(), distribution, size, False)
    print("sample = ", myPlane.generate())
except:
    import sys
    print("t_LowDiscrepancyExperiment_std.py",
          sys.exc_info()[0], sys.exc_info()[1])

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
    # Test sampling with weights
    sample, weights = myPlane.generateWithWeights()
    print("sample  = ", repr(sample))
    print("weights = ", repr(weights))
    # Test sampling with reinitialization each time the distribution is
    # set (default behaviour)
    # sample 2 != sample
    print("sample2 = ", repr(myPlane.generate()))
    myPlane.setDistribution(distribution)
    # sample 3 == sample
    print("sample3 = ", repr(myPlane.generate()))

    # Test sampling without reinitialization excepted when distribution
    # dimension changes
    myPlane = LowDiscrepancyExperiment(
        HaltonSequence(), distribution, size, False)
    print("sample  = ", repr(myPlane.generate()))
    # sample 2 != sample
    print("sample2 = ", repr(myPlane.generate()))
    myPlane.setDistribution(distribution)
    # sample 3 != sample && sample 3 != sample 2
    print("sample3 = ", repr(myPlane.generate()))
    # Test dimension change
    myPlane.setDistribution(Normal())
    print("sample = ", repr(myPlane.generate()))

    # Test constructor with no distribution and dimension>1
    myPlane = LowDiscrepancyExperiment(HaltonSequence(2), size)
    print("sample = ", repr(myPlane.generate()))
except:
    import sys
    print("t_LowDiscrepancyExperiment_std.py",
          sys.exc_info()[0], sys.exc_info()[1])

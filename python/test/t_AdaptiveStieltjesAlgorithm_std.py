#! /usr/bin/env python

from __future__ import print_function
from openturns import *
from math import *

TESTPREAMBLE()

try:
    def clean(inPoint):
        dim = inPoint.getDimension()
        for i in range(dim):
            if abs(inPoint[i]) < 1.e-10:
                inPoint[i] = 0.0
            else:
                inPoint[i] = 1e-4 * int(1e4 * inPoint[i])
        return inPoint

    iMax = 5
    # First test
    distribution = Uniform()
    algo0 = LegendreFactory()
    algo1 = AdaptiveStieltjesAlgorithm(distribution)
    print(algo1)
    for i in range(iMax):
        print(distribution.getClassName() + " Reference(", i, ")=",
              clean(algo0.getRecurrenceCoefficients(i)))
        print(distribution.getClassName() + " AdaStielj(", i, ")=",
              clean(algo1.getRecurrenceCoefficients(i)))

    # Second test
    distribution = Beta(0.5, 2.5, -1.0, 1.0)
    algo0 = JacobiFactory(0.5, 3.0, 1)
    algo1 = AdaptiveStieltjesAlgorithm(distribution)
    print(algo1)
    for i in range(iMax):
        print(distribution.getClassName() + " Reference(", i, ")=",
              clean(algo0.getRecurrenceCoefficients(i)))
        print(distribution.getClassName() + " AdaStielj(", i, ")=",
              clean(algo1.getRecurrenceCoefficients(i)))

except:
    import sys
    print("t_AdaptiveStieltjesAlgorithm_std.py",
          sys.exc_info()[0], sys.exc_info()[1])

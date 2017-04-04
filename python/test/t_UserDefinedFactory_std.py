#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    sample = Sample(3, 2)
    sample[0] = Point((1.0, 1.5))
    sample[1] = Point((2.0, 2.5))
    sample[2] = Point((3.0, 3.5))

    factory = UserDefinedFactory()
    estimatedDistribution = factory.build(sample)
    print("sample=", repr(sample))
    print("Estimated distribution=", repr(estimatedDistribution))
    estimatedDistribution = factory.build()
    print("Default distribution=", estimatedDistribution)
    estimatedUserDefined = factory.buildAsUserDefined(sample)
    print("Sample                =", sample)
    print("Estimated UserDefined=", estimatedUserDefined)
    estimatedUserDefined = factory.buildAsUserDefined()
    print("Default UserDefined=", estimatedUserDefined)
    estimatedDistribution = factory.build([[10]] * 2000)
    print("Estimated distribution=", repr(estimatedDistribution))

except:
    import sys
    print("t_UserDefinedFactory_std.py", sys.exc_info()[0], sys.exc_info()[1])

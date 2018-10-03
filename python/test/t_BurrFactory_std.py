#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()

try:
    distribution = ot.Burr(2.5, 1.5)
    size = 10000
    sample = distribution.getSample(size)
    factory = ot.BurrFactory()
    estimatedDistribution = factory.build(sample)
    print("distribution=", repr(distribution))
    print("Estimated distribution=", repr(estimatedDistribution))
    estimatedDistribution = factory.build()
    print("Default distribution=", estimatedDistribution)
    estimatedDistribution = factory.build(
        distribution.getParameter())
    print("Distribution from parameters=", estimatedDistribution)
    estimatedBurr = factory.buildAsBurr(sample)
    print("Burr          =", distribution)
    print("Estimated burr=", estimatedBurr)
    estimatedBurr = factory.buildAsBurr()
    print("Default burr=", estimatedBurr)
    estimatedBurr = factory.buildAsBurr(distribution.getParameter())
    print("Burr from parameters=", estimatedBurr)

    try:
        estimatedBurr = factory.build(ot.Normal(1e-3, 1e-5).getSample(100))
        print('Estimated burr=', estimatedBurr)
    except:
        pass

    ot.RandomGenerator.SetSeed(0)
    try:
        estimatedBurr = factory.build(ot.UserDefined(
            ot.LogNormal(7.71, 1.0056).getSample(500)).getSupport())
        #print('Estimated burr=', estimatedBurr)
    except:
        pass

except:
    import sys
    print("t_BurrFactory_std.py", sys.exc_info()[0], sys.exc_info()[1])

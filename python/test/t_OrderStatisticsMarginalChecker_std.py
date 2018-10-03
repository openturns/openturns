#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(0)


def checkMarginals(coll):
    osmc = ot.OrderStatisticsMarginalChecker(coll)
    print("marginals=", coll)
    print("isCompatible=", osmc.isCompatible())
    print("partition=", osmc.buildPartition())


coll = [ot.Uniform(-1.0, 1.0), ot.LogUniform(1.0, 1.2),
        ot.Triangular(3.0, 4.0, 5.), ot.Uniform(5.0, 6.0), ot.Uniform(5.5, 6.5)]
checkMarginals(coll)
coll.append(ot.Uniform(0.0, 1.0))
checkMarginals(coll)

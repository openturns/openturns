#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

sample = ot.Sample(3, 2)
sample[0] = ot.Point((1.0, 1.5))
sample[1] = ot.Point((2.0, 2.5))
sample[2] = ot.Point((3.0, 3.5))

factory = ot.UserDefinedFactory()
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

#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    distribution = Student(3.5, 2.5)
    size = 10000
    sample = distribution.getSample(size)
    factory = StudentFactory()
    estimatedDistribution = factory.build(sample)
    print("distribution=", repr(distribution))
    print("Estimated distribution=", repr(estimatedDistribution))
    estimatedDistribution = factory.build()
    print("Default distribution=", estimatedDistribution)
    estimatedStudent = factory.buildAsStudent(sample)
    print("Student          =", distribution)
    print("Estimated student=", estimatedStudent)
    estimatedStudent = factory.buildAsStudent()
    print("Default student=", estimatedStudent)

except:
    import sys
    print("t_StudentFactory_std.py", sys.exc_info()[0], sys.exc_info()[1])

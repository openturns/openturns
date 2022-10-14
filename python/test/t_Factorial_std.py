#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


center = ot.Point(4, 0.0)
center.setName("center")
center[0] = 0.5
center[1] = 1.5
center[2] = 2.5
center[3] = 3.5
levels = ot.Point(3, 0.0)
levels.setName("levels")
levels[0] = 4
levels[1] = 8
levels[2] = 16
experiment = ot.Factorial(center, levels)
print("experiment = ", experiment)
sample = experiment.generate()
print("sample = ", repr(sample))

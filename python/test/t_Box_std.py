#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

levels = [4] * 2
experiment = ot.Box(levels)
print('experiment = ', experiment)
sample = experiment.generate()
print('sample = ', repr(sample))

experiment = ot.Box(levels, ot.Interval([7.0, 8.0], [9.0, 16.0]))
print('experiment = ', experiment)
sample = experiment.generate()
print('sample = ', repr(sample))

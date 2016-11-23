#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

levels = [4] * 2
myPlane = ot.Box(levels)
print('myPlane = ', myPlane)
sample = myPlane.generate()
print('sample = ', repr(sample))

myPlane = ot.Box(levels, ot.Interval([7.0, 8.0], [9.0, 16.0]))
print('myPlane = ', myPlane)
sample = myPlane.generate()
print('sample = ', repr(sample))

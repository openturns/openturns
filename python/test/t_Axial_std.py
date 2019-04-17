#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    center = Point(4, 0.0)
    center.setName("center")
    center[0] = 0.5
    center[1] = 1.5
    center[2] = 2.5
    center[3] = 3.5
    levels = Point(3, 0.0)
    levels.setName("levels")
    levels[0] = 4
    levels[1] = 8
    levels[2] = 16
    experiment = Axial(center, levels)
    print("experiment = ", experiment)
    sample = experiment.generate()
    print("sample = ", repr(sample))

except:
    import sys
    print("t_Axial_std.py", sys.exc_info()[0], sys.exc_info()[1])

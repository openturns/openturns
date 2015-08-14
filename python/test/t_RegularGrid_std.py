#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    # start and step
    start = -2.0
    step = 0.1
    # Number of steps for the RegularGrid
    n = 10
    regularGrid = RegularGrid(start, step, n)

    print("regularGrid =", regularGrid)
    print("Start time =", regularGrid.getStart())
    print("End time =", regularGrid.getEnd())
    print("Time step =", regularGrid.getStep())
    print("Steps =", regularGrid.getN())

except:
    import sys
    print("t_RegularGrid_std.py", sys.exc_info()[0], sys.exc_info()[1])

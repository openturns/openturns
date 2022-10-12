#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# start and step
start = -2.0
step = 0.1
# Number of steps for the RegularGrid
n = 10
regularGrid = ot.RegularGrid(start, step, n)

print("regularGrid =", regularGrid)
print("Start time =", regularGrid.getStart())
print("End time =", regularGrid.getEnd())
print("Time step =", regularGrid.getStep())
print("Steps =", regularGrid.getN())

#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:

    # Tmin , time step and step number for TimeGrid
    Tmin = 0.0
    deltaT = 1.0
    N = 11
    timeGrid = RegularGrid(Tmin, deltaT, N)

    # Fixing the distribution for the RandomWalk
    dist = Distribution(Uniform())
    print("dist = ", dist)
    origin = Point(dist.getDimension())

    # Building a random walk
    process = RandomWalk(origin, dist)

    # print the process
    process.setTimeGrid(timeGrid)
    print("process = ", process)

    # Get a realization of the process
    timeSerie = process.getRealization()
    print("timeSerie = ", timeSerie)

    # Some steps further
    stepNumber = 4
    print("One future=", process.getFuture(stepNumber))
    size = 3
    print("Some futures=", process.getFuture(stepNumber, size))

except:
    import sys
    print("t_RandomWalk_std.py", sys.exc_info()[0], sys.exc_info()[1])

#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# Tmin , time step and step number for TimeGrid
Tmin = 0.0
deltaT = 1.0
N = 11
timeGrid = ot.RegularGrid(Tmin, deltaT, N)

# Fixing the distribution for the RandomWalk
dist = ot.Distribution(ot.Uniform())
print("dist = ", dist)
origin = ot.Point(dist.getDimension())

# Building a random walk
process = ot.RandomWalk(origin, dist)

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

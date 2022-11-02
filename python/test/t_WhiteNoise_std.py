#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# Tmin , time step and step number for TimeGrid
Tmin = 0.0
deltaT = 1.0
N = 11
timeGrid = ot.RegularGrid(Tmin, deltaT, N)

# Fixing the distribution for the WhiteNoise
dist = ot.Distribution(ot.Uniform())
print("dist = ", dist)

# Building a process from a White Noise
process = ot.Process(ot.WhiteNoise(dist))

# print the process
process.setTimeGrid(timeGrid)
print("process = ", process)

# Get a realization of the process
timeSerie = process.getRealization()
print("timeSerie = ", timeSerie)

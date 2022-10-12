#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# Time grid creation
Tmin = 0.0
deltaT = 0.1
steps = 11

# Initialization of the time grid timeGrid1
timeGrid = ot.RegularGrid(Tmin, deltaT, steps)

# Default aggregated process creation
myProcess = ot.AggregatedProcess()

# We fix the time grid to the process
myProcess.setTimeGrid(timeGrid)

print("myProcess=", myProcess)
print("a realization=", myProcess.getRealization())
marginalProcess = myProcess.getMarginal(0)
print("a marginal process=", marginalProcess)

# Parameter aggregated process creation
coll = [ot.WhiteNoise(), ot.ARMA(), ot.GaussianProcess()]
myProcess = ot.AggregatedProcess(coll)
myProcess.setTimeGrid(timeGrid)
print("myProcess=", myProcess)
print("a realization=", myProcess.getRealization())
marginalProcess = myProcess.getMarginal(1)
print("a marginal process=", marginalProcess)
print("another marginal process=", myProcess.getMarginal([0, 2]))

#! /usr/bin/env python

from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    # Time grid creation
    Tmin = 0.0
    deltaT = 0.1
    steps = 11

    # Initialization of the time grid timeGrid1
    timeGrid = RegularGrid(Tmin, deltaT, steps)

    # Default aggregated process creation
    myProcess = AggregatedProcess()

    # We fix the time grid to the process
    myProcess.setTimeGrid(timeGrid)

    print("myProcess=", myProcess)
    print("a realization=", myProcess.getRealization())
    marginalProcess = myProcess.getMarginal(0)
    print("a marginal process=", marginalProcess)

    # Parameter aggregated process creation
    coll = [WhiteNoise(), ARMA(), GaussianProcess()]
    myProcess = AggregatedProcess(coll)
    myProcess.setTimeGrid(timeGrid)
    print("myProcess=", myProcess)
    print("a realization=", myProcess.getRealization())
    marginalProcess = myProcess.getMarginal(1)
    print("a marginal process=", marginalProcess)
    print("another marginal process=", myProcess.getMarginal([0, 2]))

except:
    import sys
    print("t_AggregatedProcess_std.py", sys.exc_info()[0], sys.exc_info()[1])

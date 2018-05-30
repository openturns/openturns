#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    # TimeGrid parameters
    n = 101
    timeStart = 0.0
    timeStep = 0.1
    timeGrid = RegularGrid(timeStart, timeStep, n)

    # White noise
    whiteNoise = WhiteNoise(Uniform(), timeGrid)

    # Composite process
    process = CompositeProcess(
        ValueFunction(SymbolicFunction("x", "x+2"), timeGrid), whiteNoise)
    # A realization of the process
    timeSeries = process.getRealization()
    sample = timeSeries.getValues()

    # Now we build the factory
    factory = BoxCoxFactory()

    # Creation of the BoxCoxTransform
    myBoxCox = factory.build(timeSeries)

    print("myBoxCox (time-series)=", myBoxCox)
    print("myBoxCox (sample)     =", factory.build(sample))

    # Creation of the BoxCoxTransform using shift
    shift = Point(1, 1.0)
    myBoxCoxShift = factory.build(timeSeries, shift)

    print("myBoxCox with shift (time-series)=", myBoxCoxShift)
    print("myBoxCox with shift (sample)     =", factory.build(sample, shift))

    # Creation of the BoxCoxTransform using shift with graph
    graph = Graph()
    myBoxCoxShiftGraph = factory.build(timeSeries, shift, graph)

    print("BoxCox graph (time-series)=", graph)

except:
    import sys
    print("t_BoxCoxFactory_std.py", sys.exc_info()[0], sys.exc_info()[1])

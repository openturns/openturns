#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


# TimeGrid parameters
n = 101
timeStart = 0.0
timeStep = 0.1
timeGrid = ot.RegularGrid(timeStart, timeStep, n)

# White noise
whiteNoise = ot.WhiteNoise(ot.Uniform(), timeGrid)

# Composite process
process = ot.CompositeProcess(
    ot.ValueFunction(ot.SymbolicFunction("x", "x+2"), timeGrid), whiteNoise
)
# A realization of the process
timeSeries = process.getRealization()
sample = timeSeries.getValues()

# Now we build the factory
factory = ot.BoxCoxFactory()

# Creation of the BoxCoxTransform
myBoxCox = factory.build(timeSeries)

print("myBoxCox (time-series)=", myBoxCox)
print("myBoxCox (sample)     =", factory.build(sample))

# Creation of the BoxCoxTransform using shift
shift = ot.Point(1, 1.0)
myBoxCoxShift, graph = factory.buildWithGraph(timeSeries, shift)

print("myBoxCox with shift (time-series)=", myBoxCoxShift)
print("myBoxCox with shift (sample)     =", factory.build(sample, shift))
print("BoxCox graph (time-series)=", graph)

# with sample
myBoxCoxShift, graph = factory.buildWithGraph(sample, shift)

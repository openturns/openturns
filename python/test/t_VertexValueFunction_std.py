#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


# Create an intance
myFunc = ot.SymbolicFunction(["t", "x"], ["x + t^2"])
tg = ot.RegularGrid(0.0, 0.2, 6)
myTemporalFunc = ot.VertexValueFunction(myFunc, tg)

print("myTemporalFunc=", myTemporalFunc)
# Get the input and output description
print("myTemporalFunc input description=",
      myTemporalFunc.getInputDescription())
print("myTemporalFunc output description=",
      myTemporalFunc.getOutputDescription())
# Get the input and output dimension, based on description
print("myTemporalFunc input dimension=",
      myTemporalFunc.getInputDimension())
print("myTemporalFunc output dimension=",
      myTemporalFunc.getOutputDimension())
# Create a TimeSeries
data = ot.Sample(tg.getN(), myFunc.getInputDimension() - 1)
for i in range(data.getSize()):
    for j in range(data.getDimension()):
        data[i, j] = i * data.getDimension() + j
ts = ot.TimeSeries(tg, data)
print("input time series=", ts)
print("output time series=", myTemporalFunc(ts))
# Get the number of calls
print("called ", myTemporalFunc.getCallsNumber(), " times")

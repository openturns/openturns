#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


inputVars = ot.Description(["t"])
formula = ot.Description(["sin(t)", "cos(t)"])
myFunc = ot.SymbolicFunction(inputVars, formula)
tg = ot.RegularGrid(0.0, 0.1, 11)
myTrendFunc = ot.TrendTransform(myFunc, tg)

print("myTrendFunc=", myTrendFunc)
# Get the input description and dimension
print("myTrendFunc input description=", myTrendFunc.getInputDescription())
print("myTrendFunc input dimension=", myTrendFunc.getInputDimension())
# Get the output description and dimension
print("myTrendFunc output description=", myTrendFunc.getOutputDescription())
print("myTrendFunc output dimension=", myTrendFunc.getOutputDimension())
# Create a TimeSeries
data = ot.Sample(tg.getN(), formula.getSize())
for i in range(data.getSize()):
    for j in range(data.getDimension()):
        data[i, j] = i * data.getDimension() + j
ts = ot.TimeSeries(tg, data)
print("input time series =  ")
print(ts)
print("output time series = ")
print(myTrendFunc(ts))
# Get the number of calls
print("called ", myTrendFunc.getCallsNumber(), " times")

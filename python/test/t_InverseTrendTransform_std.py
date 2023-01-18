#! /usr/bin/env python

import openturns as ot
import math as m

ot.TESTPREAMBLE()


inputVars = ot.Description(["t"])
formula = ot.Description(["sin(t)", "cos(t)"])
myFunc = ot.SymbolicFunction(inputVars, formula)
tg = ot.RegularGrid(0.0, 0.1, 11)
myInverseTrendFunc = ot.InverseTrendTransform(myFunc, tg)

print("myInverseTrendFunc=", myInverseTrendFunc)
# Get the input description and dimension
print("myInverseTrendFunc input description=", myInverseTrendFunc.getInputDescription())
print("myInverseTrendFunc input dimension=", myInverseTrendFunc.getInputDimension())
# Get the output description and dimension
print(
    "myInverseTrendFunc output description=", myInverseTrendFunc.getOutputDescription()
)
print("myInverseTrendFunc output dimension=", myInverseTrendFunc.getOutputDimension())
# Create a TimeSeries
data = ot.Sample(tg.getN(), formula.getSize())
for i in range(data.getSize()):
    t = tg.getStart() + i * tg.getStep()
    data[i, 0] = i + 1 + m.sin(t)
    data[i, 1] = i + m.cos(t)
ts = ot.TimeSeries(tg, data)
print("input time series =  ")
print(ts)
print("output time series = ")
print(myInverseTrendFunc(ts))
# Get the number of calls
print("called ", myInverseTrendFunc.getCallsNumber(), " times")

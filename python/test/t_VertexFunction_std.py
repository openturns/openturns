#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    # Create an intance
    myFunc = SymbolicFunction("t", "t + t^2")
    myVertexFunc = VertexFunction(myFunc)

    print("myVertexFunc=", myVertexFunc)
    # Get the input and output description
    print("myVertexFunc input description=", myVertexFunc.getInputDescription())
    print("myVertexFunc output description=", myVertexFunc.getOutputDescription())
    # Get the input and output dimension, based on description
    print("myVertexFunc input dimension=", myVertexFunc.getInputDimension())
    print("myVertexFunc output dimension=", myVertexFunc.getOutputDimension())
    # Create a TimeSeries
    tg = RegularGrid(0.0, 0.2, 6)
    data = Sample(tg.getN(), myFunc.getInputDimension() - 1)
    for i in range(data.getSize()):
        for j in range(data.getDimension()):
            data[i, j] = i * data.getDimension() + j
    ts = TimeSeries(tg, data)
    print("input time series=", ts)
    print("output time series=", myVertexFunc(ts))
    # Get the number of calls
    print("called ", myVertexFunc.getCallsNumber(), " times")

except:
    import sys
    print("t_VertexFunction_std.py", sys.exc_info()[0], sys.exc_info()[1])

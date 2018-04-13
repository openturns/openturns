#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    # Create an intance
    myFunc = SymbolicFunction("x", "x^2")
    tg = RegularGrid(0.0, 0.2, 6)
    mySpatialFunc = ValueFunction(myFunc, tg)

    print("mySpatialFunc=", mySpatialFunc)
    # Get the input and output description
    print("mySpatialFunc input description=",
          mySpatialFunc.getInputDescription())
    print("mySpatialFunc output description=",
          mySpatialFunc.getOutputDescription())
    # Get the input and output dimension, based on description
    print("mySpatialFunc input dimension=", mySpatialFunc.getInputDimension())
    print("mySpatialFunc output dimension=",
          mySpatialFunc.getOutputDimension())
    # Create a TimeSeries
    data = Sample(tg.getN(), myFunc.getInputDimension())
    for i in range(data.getSize()):
        for j in range(data.getDimension()):
            data[i, j] = i * data.getDimension() + j
    ts = TimeSeries(tg, data)
    print("input time series=", ts)
    print("output time series=", mySpatialFunc(ts))
    # Get the number of calls
    print("called ", mySpatialFunc.getCallsNumber(), " times")

except:
    import sys
    print("t_ValueFunction_std.py", sys.exc_info()[0], sys.exc_info()[1])

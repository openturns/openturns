#! /usr/bin/env python

from __future__ import print_function
from openturns import *
from math import *

TESTPREAMBLE()

try:
    # Create an intance
    inputVars = Description(["t"])
    formula = Description(["sin(t)", "cos(t)"])
    myFunc = SymbolicFunction(inputVars, formula)
    tg = RegularGrid(0.0, 0.1, 11)
    myInverseTrendFunc = InverseTrendTransform(myFunc, tg)

    print("myInverseTrendFunc=", myInverseTrendFunc)
    # Get the input description and dimension
    print("myInverseTrendFunc input description=",
          myInverseTrendFunc.getInputDescription())
    print("myInverseTrendFunc input dimension=",
          myInverseTrendFunc.getInputDimension())
    # Get the output description and dimension
    print("myInverseTrendFunc output description=",
          myInverseTrendFunc.getOutputDescription())
    print("myInverseTrendFunc output dimension=",
          myInverseTrendFunc.getOutputDimension())
    # Create a TimeSeries
    data = Sample(tg.getN(), formula.getSize())
    for i in range(data.getSize()):
        t = tg.getStart() + i * tg.getStep()
        data[i, 0] = i + 1 + sin(t)
        data[i, 1] = i + cos(t)
    ts = TimeSeries(tg, data)
    print("input time series =  ")
    print(ts)
    print("output time series = ")
    print(myInverseTrendFunc(ts))
    # Get the number of calls
    print("called ", myInverseTrendFunc.getCallsNumber(), " times")

except:
    import sys
    print("t_InverseTrendTransform_std.py",
          sys.exc_info()[0], sys.exc_info()[1])

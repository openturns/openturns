#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    # Create an intance
    myFunc = PointToFieldFunction()

    print("myFunc=", myFunc)
    # Get the input and output description
    print("myFunc input description=", myFunc.getInputDescription())
    print("myFunc output description=", myFunc.getOutputDescription())
    # Get the input and output dimension
    print("myFunc input dimension=", myFunc.getInputDimension())
    print("myFunc output dimension=", myFunc.getOutputDimension())
    # Get the number of calls
    print("called ", myFunc.getCallsNumber(), " times")

except:
    import sys
    print("t_PointToFieldFunction_std.py",
          sys.exc_info()[0], sys.exc_info()[1])

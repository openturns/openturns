#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    # Create an intance
    myFunc = FieldToPointFunction()

    print("myFunc=", myFunc)
    # Get the input and output description
    print("myFunc input description=", myFunc.getInputDescription())
    print("myFunc output description=", myFunc.getOutputDescription())
    # Get the spatial, input and output dimension
    print("myFunc input dimension=", myFunc.getInputMesh().getDimension())
    print("myFunc input dimension=", myFunc.getInputDimension())
    print("myFunc output dimension=", myFunc.getOutputDimension())
    # Get the number of calls
    print("called ", myFunc.getCallsNumber(), " times")

except:
    import sys
    print("t_FieldToPointFunction.py", sys.exc_info()[0], sys.exc_info()[1])

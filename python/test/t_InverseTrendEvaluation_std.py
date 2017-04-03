#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:

    # Dimension of test
    dimension = 2

    # function
    function = SymbolicFunction("x", "sin(x)")

    # Init of the function
    myFunction = InverseTrendEvaluation(function)
    myFunction.setName("myInverseTrendFunction")

    # We check that for a Point with same values, the function is
    # done by component
    inPoint = Point(dimension, 30.0)

    # result point of the function
    outPoint = myFunction(inPoint)
    print("myFunction=", myFunction)
    print(myFunction.getName(), "( ", inPoint, " ) = ", outPoint)

    # Creation of a Sample
    size = 10
    inSample = Sample(size, dimension)
    for index in range(size):
        inSample[index] = Point(dimension, index + 1)

    # result of the function
    outSample = myFunction(inSample)
    print(myFunction.getName(), "( ", repr(inSample), " ) = ", repr(outSample))


except:
    import sys
    print("t__InverseTrendEvaluation_std.py",
          sys.exc_info()[0], sys.exc_info()[1])

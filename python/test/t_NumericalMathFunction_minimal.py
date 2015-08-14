#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:

    # Instance creation
    smallest = NumericalMathFunction("minimal_wrapper")

    inPoint = NumericalPoint(smallest.getInputDimension())
    inPoint[0] = 2
    inPoint[1] = 3

    outPoint = smallest(inPoint)

    print("smallest =", repr(outPoint))

except:
    import sys
    print("t_NumericalMathFunction_minimal.py",
          sys.exc_info()[0], sys.exc_info()[1])

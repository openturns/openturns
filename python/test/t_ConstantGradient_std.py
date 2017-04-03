#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:

    inputDimension = 3
    outputDimension = 2
    # Constant term
    constant = Matrix(inputDimension, outputDimension)
    constant[0, 0] = 1.0
    constant[1, 0] = 2.0
    constant[2, 0] = 5.0
    constant[0, 1] = 7.0
    constant[1, 1] = 9.0
    constant[2, 1] = 3.0

    myGradient = ConstantGradient(constant)
    myGradient.setName("constantGradient")
    inPoint = Point(inputDimension)
    inPoint[0] = 7.0
    inPoint[1] = 8.0
    inPoint[2] = 9.0
    outMatrix = myGradient.gradient(inPoint)
    print("myGradient=", repr(myGradient))
    print(myGradient.getName(), "( ", repr(inPoint), " ) = ", repr(outMatrix))

except:
    import sys
    print("t_ConstantGradient_std.py",
          sys.exc_info()[0], sys.exc_info()[1])

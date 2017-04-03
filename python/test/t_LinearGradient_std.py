#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    inputDimension = 3
    outputDimension = 2
    # Center
    center = Point(inputDimension)
    center[0] = -1
    center[1] = 0.5
    center[2] = 1
    # Constant term
    constant = Matrix(inputDimension, outputDimension)
    constant[0, 0] = 1.0
    constant[1, 0] = 2.0
    constant[2, 0] = 5.0
    constant[0, 1] = 7.0
    constant[1, 1] = 9.0
    constant[2, 1] = 3.0
    # Linear term
    linear = SymmetricTensor(inputDimension, outputDimension)
    linear[0, 0, 0] = 7.0
    linear[0, 0, 1] = -7.0
    linear[0, 1, 0] = 8.0
    linear[0, 1, 1] = -8.0
    linear[0, 2, 0] = 9.0
    linear[0, 2, 1] = -9.0
    linear[1, 0, 0] = 8.0
    linear[1, 0, 1] = -8.0
    linear[1, 1, 0] = 10.0
    linear[1, 1, 1] = -10.0
    linear[1, 2, 0] = 11.0
    linear[1, 2, 1] = -11.0
    linear[2, 0, 0] = 9.0
    linear[2, 0, 1] = -9.0
    linear[2, 1, 0] = 11.0
    linear[2, 1, 1] = -11.0
    linear[2, 2, 0] = 12.0
    linear[2, 2, 1] = -12.0

    # myFunction = linear * (X- center) + constant
    myGradient = LinearGradient(
        center, constant, linear)
    myGradient.setName("linearGradient")
    inPoint = Point(inputDimension)
    inPoint[0] = 7.0
    inPoint[1] = 8.0
    inPoint[2] = 9.0
    outMatrix = myGradient.gradient(inPoint)
    print("myGradient=", repr(myGradient))
    print(myGradient.getName(), "( ", repr(inPoint), " ) = ", repr(outMatrix))

except:
    import sys
    print("t_LinearGradient_std.py",
          sys.exc_info()[0], sys.exc_info()[1])

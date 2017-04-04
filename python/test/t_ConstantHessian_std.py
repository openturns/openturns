#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:

    inputDimension = 3
    outputDimension = 2

    # Constant term
    constant = SymmetricTensor(inputDimension, outputDimension)
    constant[0, 0, 0] = 7.0
    constant[0, 0, 1] = -7.0
    constant[0, 1, 0] = 8.0
    constant[0, 1, 1] = -8.0
    constant[0, 2, 0] = 9.0
    constant[0, 2, 1] = -9.0
    constant[1, 0, 0] = 8.0
    constant[1, 0, 1] = -8.0
    constant[1, 1, 0] = 10.0
    constant[1, 1, 1] = -10.0
    constant[1, 2, 0] = 11.0
    constant[1, 2, 1] = -11.0
    constant[2, 0, 0] = 9.0
    constant[2, 0, 1] = -9.0
    constant[2, 1, 0] = 11.0
    constant[2, 1, 1] = -11.0
    constant[2, 2, 0] = 12.0
    constant[2, 2, 1] = -12.0

    myHessian = ConstantHessian(constant)
    myHessian.setName("constantHessian")
    inPoint = Point(inputDimension)
    inPoint[0] = 7.0
    inPoint[1] = 8.0
    inPoint[2] = 9.0
    outTensor = myHessian.hessian(inPoint)
    print("myHessian=", repr(myHessian))
    print(myHessian.getName(), "( ", repr(inPoint), " ) = ", repr(outTensor))

except:
    import sys
    print("t_ConstantHessian_std.py",
          sys.exc_info()[0], sys.exc_info()[1])

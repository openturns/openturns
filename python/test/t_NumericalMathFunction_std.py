#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:

    # Instance creation
    myFunc = NumericalMathFunction(
        ['x1', 'x2'], ['y1', 'y2', 'y3'], ['x1*sin(x2)', 'cos(x1+x2)', '(x2+1)*exp(x1-2*x2)'])

    # Copy constructor
    newFunc = NumericalMathFunction(myFunc)

    print("myFunc=" + repr(myFunc))
    print("myFunc input parameter(s)=")
    for i in range(myFunc.getInputDimension()):
        print(myFunc.getInputDescription()[i])
    print("myFunc output parameter(s) and marginal(s)=")
    for i in range(myFunc.getOutputDimension()):
        print(myFunc.getOutputDescription()[i])
        print("Marginal function", i, "=", repr(myFunc.getMarginal(i)))

except:
    import sys
    print("t_NumericalMathFunction_std.py",
          sys.exc_info()[0], sys.exc_info()[1])

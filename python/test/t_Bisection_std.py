#! /usr/bin/env python

from __future__ import print_function
from openturns import *
from math import *

TESTPREAMBLE()

try:

    # Analytical construction
    input = Description(1)
    input[0] = "x"
    formulas = Description(1)
    formulas[0] = "x-cos(x)"
    analytical = SymbolicFunction(input, formulas)
    myAlgo = Bisection()
    print("myAlgo=", myAlgo)
    value = 0.0
    print("Solve ", formulas[0], "=%.5f" % value, " for ", input[0])
    print("x=%.5f" % myAlgo.solve(analytical, value, 0.0, 3.0))
    print("myAlgo=", myAlgo)

except:
    import sys
    print("t_Bisection_std.py", sys.exc_info()[0], sys.exc_info()[1])

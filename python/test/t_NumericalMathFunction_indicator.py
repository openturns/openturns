#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:

    # Analytical construction
    input = Description(2)
    input[0] = "x0"
    input[1] = "x1"
    output = Description(1)
    output[0] = "y"
    formulas = Description(output.getSize())
    formulas[0] = "x0+x1"
    analytical = NumericalMathFunction(input, output, formulas)

    print("function=", repr(analytical))

    # Create indicator function
    indicator = NumericalMathFunction(
        analytical, Less(), 0.0)
    # Does it work?
    x = NumericalPoint(NumericalPoint(2, 1.0))
    value = analytical(x)[0]
    print("Value of the function=", repr(value),
          " value of the indicator=", repr(indicator(x)))
    x = NumericalPoint(2, -1.0)
    value = analytical(x)[0]
    print("Value of the function=", repr(value),
          " value of the indicator=", repr(indicator(x)))

except:
    import sys
    print("t_NumericalMathFunction_indicator.py",
          sys.exc_info()[0], sys.exc_info()[1])

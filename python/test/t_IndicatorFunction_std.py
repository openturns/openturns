#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:

    # Analytical construction
    analytical = SymbolicFunction(["x0", "x1"], ["x0+x1"])
    print("function=", repr(analytical))

    # Create indicator function
    indicator = IndicatorFunction(
        analytical, Less(), 0.0)
    # Does it work?
    x = Point(2, 1.0)
    value = analytical(x)[0]
    print("Value of the function=", repr(value),
          " value of the indicator=", repr(indicator(x)))
    x = Point(2, -1.0)
    value = analytical(x)[0]
    print("Value of the function=", repr(value),
          " value of the indicator=", repr(indicator(x)))

except:
    import sys
    print("t_Function_indicator.py",
          sys.exc_info()[0], sys.exc_info()[1])

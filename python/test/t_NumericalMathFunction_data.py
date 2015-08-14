#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:

    # Database construction
    inputSample = NumericalSample(0, 2)
    inputSample.add(NumericalPoint(2, 1.0))
    inputSample.add(NumericalPoint(2, 2.0))
    inputSample.setDescription(["x0", "x1"])
    outputSample = NumericalSample(0, 1)
    outputSample.add(NumericalPoint(1, 4.0))
    outputSample.add(NumericalPoint(1, 5.0))
    outputSample.setDescription(["y0"])
    database = NumericalMathFunction(inputSample, outputSample)

    print("database=", database)

    # Does it work?
    x = NumericalPoint(database.getInputDimension(), 1.8)
    print("x=", x)
    print("database(x)=", database(x))

except:
    import sys
    print("t_NumericalMathFunction_data.py",
          sys.exc_info()[0], sys.exc_info()[1])

#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:

    # Instance creation
    deviation = NumericalMathFunction("poutre")

    inPoint1 = NumericalPoint(4)
    inPoint1[0] = 210.e9
    inPoint1[1] = 1000
    inPoint1[2] = 1.4
    inPoint1[3] = 2.e-6

    inPoint2 = NumericalPoint(4)
    inPoint2[0] = 210.e9
    inPoint2[1] = 1000
    inPoint2[2] = 1.5
    inPoint2[3] = 2.e-6

    inPoint3 = NumericalPoint(4)
    inPoint3[0] = 210.e9
    inPoint3[1] = 1000
    inPoint3[2] = 1.6
    inPoint3[3] = 2.e-6

    outPoint = deviation(inPoint2)
    print("deviation =", repr(outPoint))

    # Test samples
    inSample = NumericalSample(3, deviation.getInputDimension())
    inSample[0] = inPoint1
    inSample[1] = inPoint2
    inSample[2] = inPoint3

    outSample = deviation(inSample)
    print("deviation sample = ", repr(outSample))

    # Test timeseries
    inTimeSeries = TimeSeries(3, deviation.getInputDimension())
    inTimeSeries.setValueAtIndex(0, inPoint1)
    inTimeSeries.setValueAtIndex(1, inPoint2)
    inTimeSeries.setValueAtIndex(2, inPoint3)

    outTimeSeries = deviation(inTimeSeries)
    print("deviation time series = ", repr(outTimeSeries))

    # Test gradient method
    gradient = deviation.gradient(inPoint2)
    print("deviation.gradient = ", repr(gradient))

    # Test hessian method
    tensor = deviation.hessian(inPoint2)
    print("deviation.hessian = ", repr(tensor))

    # Test wrapper error
    try:
        inPoint = NumericalPoint(4)
        inPoint[0] = 210.e9  # E
        inPoint[1] = 1000    # F
        inPoint[2] = 1.5     # L
        inPoint[3] = 0.      # I is zero so an error should be raised

        outPoint = deviation(inPoint)

        # Normally, we should never go here
        raise

    except:
        pass


except:
    import sys
    print("t_NumericalMathFunction_exec.py",
          sys.exc_info()[0], sys.exc_info()[1])

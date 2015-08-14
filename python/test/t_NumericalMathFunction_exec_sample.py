#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    # Instance creation
    deviation = NumericalMathFunction("poutre_sample")

    input = Description(4)
    input[0] = "E"
    input[1] = "F"
    input[2] = "L"
    input[3] = "I"
    output = Description(1)
    output[0] = "d"
    formula = Description(output.getSize())
    formula[0] = "-F*L^3/(3*E*I)"
    deviation_analytical = NumericalMathFunction(input, output, formula)

    size = 10
    inSample = NumericalSample(0, 4)
    for i in range(size):
        fact = (1.0 + float(i) / size)
        inSample.add(NumericalPoint((210.e9, 1000.0, 1.5, 2.e-6)) * fact)

    outSample1 = deviation(inSample)
    outSample2 = deviation.getEvaluation()(inSample)
    outSample3 = deviation_analytical(inSample)
    print("outSample by sample evaluation=", repr(outSample1))
    print("outSample by point evaluation=", repr(outSample2))
    print("outSample by analytical function=", repr(outSample3))

except:
    import sys
    print("t_NumericalMathFunction_exec_sample.py",
          sys.exc_info()[0], sys.exc_info()[1])

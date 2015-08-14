#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    evaluation = AnalyticalNumericalMathEvaluationImplementation(
        ["x0", "x1", "x2"], ["y0"], ["x0^2+2*x1+3*x2^3"])
    hessian = AnalyticalNumericalMathHessianImplementation(evaluation)
    point = [-1.0, 4.0, -4.0]
    print("hessian=", hessian)
    print("value at", point, "=", hessian.hessian(point))

except:
    import sys
    print("t_AnalyticalNumericalMathHessianImplementation_std.py",
          sys.exc_info()[0], sys.exc_info()[1])

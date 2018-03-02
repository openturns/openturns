#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    ResourceMap.Set("SymbolicParser-Backend", "ExprTk")
    evaluation = SymbolicEvaluation(
        ["x0", "x1", "x2"], ["y0"], ["x0^2+2*x1+3*x2^3"])
    hessian = SymbolicHessian(evaluation)
    point = [-1.0, 4.0, -4.0]
    print("hessian=", hessian)
    print("value at", point, "=", hessian.hessian(point))

except:
    import sys
    print("t_SymbolicHessian_std.py",
          sys.exc_info()[0], sys.exc_info()[1])

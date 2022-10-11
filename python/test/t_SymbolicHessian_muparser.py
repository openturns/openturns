#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


ot.ResourceMap.Set("SymbolicParser-Backend", "MuParser")
evaluation = ot.SymbolicEvaluation(["x0", "x1", "x2"], ["y0"], ["x0^2+2*x1+3*x2^3"])
hessian = ot.SymbolicHessian(evaluation)
point = [-1.0, 4.0, -4.0]
print("hessian=", hessian)
print("value at", point, "=", hessian.hessian(point))

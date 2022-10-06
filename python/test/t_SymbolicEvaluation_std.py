#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


ot.ResourceMap.Set("SymbolicParser-Backend", "ExprTk")
evaluation = ot.SymbolicEvaluation(
    ["x0", "x1", "x2"], ["y0", "y1"], ["x0^2+2*x1+3*x2^3", "cos(x0-sin(x1 * x2))"])
point = [-1.0, 4.0, -4.0]
print("evaluation=", evaluation)
print("value at ", point, "=", evaluation(point))
for i in range(evaluation.getOutputDimension()):
    print("marginal", i, "=", evaluation.getMarginal(i))


#! /usr/bin/env python

import openturns as ot

function1 = ot.SymbolicFunction(["x0", "x1", "x2"], ["x0^2+2*x1+3*x2^3"])
function2 = ot.SymbolicFunction(
    ["x0", "x1", "x2"], ["cos(x0*sin(x2+x1))", "exp(x1 - x0 * sin(x2))"]
)
evaluation = ot.AggregatedEvaluation([function1, function2])
print("evaluation=", evaluation)
point = [4.0, -4.0, 1.0]
print("function 1 at", point, "=", function1(point))
print("function 2 at", point, "=", function2(point))
print("evaluation at", point, "=", evaluation(point))

# Equality tests
e1 = ot.AggregatedEvaluation([function1])
e2 = ot.AggregatedEvaluation([function1])
assert e1 == e2, "same functions should be equal"
assert e1 != evaluation, "different number of functions"
e3 = ot.AggregatedEvaluation([function2])
assert e1 != e3, "different functions"
e4 = ot.AggregatedEvaluation([function1, function2])
assert evaluation == e4, "same pair of functions"

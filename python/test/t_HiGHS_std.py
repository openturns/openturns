#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

# inspired from highs example: https://github.com/ERGO-Code/HiGHS/blob/master/examples/call_highs_from_cpp.cpp
# min f = 1.1*x_0 + x_1
# ineq:
#        x_1 <= 7
#   5 <= x_0 + 2x_1 <= 15
#   6 <= 3x_0 + 2x_1
# bounds:
#   0 <= x_0 <= 4; 1 <= x_1
ot.ResourceMap.AddAsBool("HiGHS-output_flag", True)
bounds = ot.Interval([0.0, 1.0], [4.0, 1e30])
cost = [1.1, 1.0]
A = ot.Matrix([[0.0, 1.0], [1.0, 2.0], [3.0, 2.0]])
cb = ot.Interval([-1e9, 5.0, 6.0], [7.0, 15.0, 1e9])
problem = otexp.LinearProblem(cost, bounds, A, cb)
print(problem)

sol = {ot.OptimizationProblemImplementation.CONTINUOUS: [0.5, 2.25],
       ot.OptimizationProblemImplementation.INTEGER: [0.0, 3.0]}
for vtype in [ot.OptimizationProblemImplementation.CONTINUOUS,
              ot.OptimizationProblemImplementation.INTEGER]:
    problem.setVariablesType([vtype] * 2)
    algo = otexp.HiGHS(problem)
    print(algo)
    algo.run()
    result = algo.getResult()
    print(result)
    x = result.getOptimalPoint()
    ott.assert_almost_equal(x, sol[vtype])

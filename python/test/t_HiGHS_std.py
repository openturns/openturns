#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

# inspired from highs example: https://github.com/ERGO-Code/HiGHS/blob/master/examples/call_highs_from_cpp.cpp
ot.ResourceMap.AddAsBool("HiGHS-output_flag", True)
f = ot.SymbolicFunction(["x0", "x1"], ["1.1*x0+x1+3"])
bounds = ot.Interval([0.0, 1.0], [4.0, 1e30])
ineq = ot.SymbolicFunction(["x0", "x1"], ["7-x1", "x0+2*x1-5", "15-x0-2*x1", "3*x0+2*x1-6"])
problem = ot.OptimizationProblem(f)
problem.setBounds(bounds)
problem.setInequalityConstraint(ineq)
for vtype in [ot.OptimizationProblemImplementation.CONTINUOUS,
              ot.OptimizationProblemImplementation.INTEGER]:
    problem.setVariablesType([vtype] * 2)
    algo = otexp.HiGHS(problem)
    print(algo)
    algo.run()
    result = algo.getResult()
    print(result)
    x = result.getOptimalPoint()
    sol = {ot.OptimizationProblemImplementation.CONTINUOUS: [0.5, 2.25],
           ot.OptimizationProblemImplementation.INTEGER: [0.0, 3.0]}
    ott.assert_almost_equal(x, sol[vtype])

#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

dim = 2
f = ot.SymbolicFunction(["x1", "x2"], ["x1^2+x2^2"])
x0 = [0.5] * dim
bounds = ot.Interval([-1.0, -10.0], [10.0, 1.0])

for minimization in [True, False]:
    for ineq in [False, True]:
        problem = ot.OptimizationProblem(f)
        problem.setBounds(bounds)
        problem.setMinimization(minimization)
        if ineq:
            h = ot.SymbolicFunction(["x1", "x2"], ["5-x1", "x2+5"])
            problem.setInequalityConstraint(h)
        algoNames = ot.OptimizationAlgorithm.GetAlgorithmNames(problem)
        for algoName in algoNames:
            algo = ot.OptimizationAlgorithm.Build(algoName)
            algo.setProblem(problem)
            algo.setMaximumConstraintError(1e-1)
            algo.setMaximumCallsNumber(1000)
            try:
                algo.setStartingPoint(x0)
            except Exception:
                # multi-obj
                continue
            print(f"minimization={minimization} ineq={ineq} algo={algoName}")
            algo.run()
            result = algo.getResult()
            x = result.getOptimalPoint()
            y = result.getOptimalValue()
            print(f"x^={x} y^={y}")
            if minimization:
                assert abs(y[0]) < 0.3
            else:
                ref = 25.0 if ineq else 100.0
                assert y[0] > ref

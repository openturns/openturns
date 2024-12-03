#! /usr/bin/env pythons

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

# Analytical construction */
inputs = ["x", "y"]
formulas = ["y*x-sin(2*x)", "1 + cos(y) + x"]
analytical = ot.SymbolicFunction(inputs, formulas)
algo = otexp.LeastSquaresEquationsSolver()
algo.setResidualError(1e-10)
algo.setMaximumCallsNumber(1000)
starting_point = [2.0, 1.0]
print("Solve ", formulas, "= [0,0] for ", inputs)
solution = algo.solve(analytical, starting_point)
print("[x,y] = ", solution)
print("algo=", algo)
ott.assert_almost_equal(analytical(solution), [0, 0], 1e-5, 1e-5)

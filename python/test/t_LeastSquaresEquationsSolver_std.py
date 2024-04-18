#! /usr/bin/env pythons

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

# Analytical construction */
input = ot.Description(2)
input = ["x", "y"]
formulas = ot.Description(2)
formulas = ["y*x-sin(2*x)", "1 + cos(y) + x"]
analytical = ot.SymbolicFunction(input, formulas)
myAlgo = otexp.LeastSquaresEquationsSolver()
myAlgo.setResidualError(1e-10)
myAlgo.setMaximumCallsNumber(1000)
print("myAlgo=", myAlgo)
starting_point = ot.Point([2.0, 1.0])
print("Solve ", formulas, "= [0,0] for ", input)
solution = myAlgo.solve(analytical, starting_point)
print("[x,y] = ", solution)
print("myAlgo=", myAlgo)
ott.assert_almost_equal(analytical(solution), [0, 0], 1e-5, 1e-5)

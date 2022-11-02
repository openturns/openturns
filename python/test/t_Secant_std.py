#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# Analytical construction */
input = ot.Description(1)
input[0] = "x"
formulas = ot.Description(1)
formulas[0] = "x-cos(2*x)"
analytical = ot.SymbolicFunction(input, formulas)
myAlgo = ot.Secant()
print("myAlgo=", myAlgo)
value = 0.0
print("Solve ", formulas[0], "=%.5f" % value, " for ", input[0])
print("x=%.5f" % myAlgo.solve(analytical, value, 0.0, 3.0))
print("myAlgo=", myAlgo)

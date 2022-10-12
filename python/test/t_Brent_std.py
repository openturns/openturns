#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# Analytical construction
input = "x"
formulas = "x-cos(2*x)"
analytical = ot.SymbolicFunction(["x"], [formulas])
myAlgo = ot.Brent()
print("myAlgo=", myAlgo)
value = 0.0
print("Solve ", formulas, "=%.5f" % value, " for ", input)
print("x=%.5f" % myAlgo.solve(analytical, value, -4.0, 4.0 / 3.0))
print("myAlgo=", myAlgo)

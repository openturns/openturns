#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# Analytical construction
input = "x"
formulas = "x-cos(2*x)"
analytical = ot.SymbolicFunction([input], [formulas])
myAlgo = ot.Bisection()
print("myAlgo=", myAlgo)
value = 0.0
print("Solve ", formulas, "=%.5f" % value, " for ", input)
print("x=%.5f" % myAlgo.solve(analytical, value, 0.0, 3.0))
print("myAlgo=", myAlgo)

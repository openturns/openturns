#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


# Simplified interfaces
# First, try 1D drawing
f = ot.SymbolicFunction("x", "sin(2*pi_*x)*exp(-x^2/2)")
print("f=", f)
graph = f.draw(-1.2, 1.2, 32)
print("graph=", graph)

# Second, try 2D drawing
f = ot.SymbolicFunction(["x", "y"], ["2.0+x-2*y+x*y-x^2-3*y^2+x*y^2"])
print("f=", f)
graph = f.draw(ot.Point(2, -10.0), ot.Point(2, 10.0), ot.Indices(2, 21))
print("graph=", graph)

# Full interfaces
f = ot.SymbolicFunction(
    ["x0", "x1", "x2"],
    [
        "x0 * sin(x1 + 2.4 * x2) - 2.0 * x1 * cos(3.2 * x0 - x2)",
        "x1 * cos(x2 + 2.4 * x1) + 2.0 * x0 * cos(3.2 * x1 - x0)",
    ],
)
centralPoint = ot.Point([1.0, -0.5, 1.5])
# First output as a function of first input around central point
graph1D = f.draw(0, 0, centralPoint, -5.0, 5.0, 32)
print("graph1D=", graph1D)
# Second output as a function of second and third inputs around central
# point
graph2D = f.draw(
    1, 2, 1, centralPoint, ot.Point(2, -5.0), ot.Point(2, 5.0), ot.Indices(2, 21)
)
print("graph2D=", graph2D)

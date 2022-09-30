#! /usr/bin/env python

from openturns import *

TESTPREAMBLE()

try:
    # Simplified interfaces
    # First, try 1D drawing
    f = SymbolicFunction("x", "sin(2*pi_*x)*exp(-x^2/2)")
    print("f=", f)
    graph = f.draw(-1.2, 1.2, 32)
    print("graph=", graph)

    # Second, try 2D drawing
    f = SymbolicFunction(
        ["x", "y"], ["2.0+x-2*y+x*y-x^2-3*y^2+x*y^2"])
    print("f=", f)
    graph = f.draw(Point(2, -10.0),
                   Point(2, 10.0), Indices(2, 21))
    print("graph=", graph)

    # Full interfaces
    f = SymbolicFunction(["x0", "x1", "x2"], [
        "x0 * sin(x1 + 2.4 * x2) - 2.0 * x1 * cos(3.2 * x0 - x2)", "x1 * cos(x2 + 2.4 * x1) + 2.0 * x0 * cos(3.2 * x1 - x0)"])
    centralPoint = Point([1.0, -0.5, 1.5])
    # First output as a function of first input around central point
    graph1D = f.draw(0, 0, centralPoint, -5.0, 5.0, 32)
    print("graph1D=", graph1D)
    # Second output as a function of second and third inputs around central
    # point
    graph2D = f.draw(1, 2, 1, centralPoint, Point(
        2, -5.0), Point(2, 5.0), Indices(2, 21))
    print("graph2D=", graph2D)

except:
    import sys
    print("t_Function_draw.py",
          sys.exc_info()[0], sys.exc_info()[1])

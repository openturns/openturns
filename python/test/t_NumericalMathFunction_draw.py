#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    # Simplified interfaces
    # First, try 1D drawing
    f = NumericalMathFunction("x", "sin(2*_pi*x)*exp(-x^2/2)", "y")
    print("f=", f)
    graph = f.draw(-1.2, 1.2, 32)
    print("graph=", graph)
    graph.draw("NMFDraw1D.png")

    # Second, try 2D drawing
    f = NumericalMathFunction(
        ["x", "y"], ["z"], ["2.0+x-2*y+x*y-x^2-3*y^2+x*y^2"])
    print("f=", f)
    graph = f.draw(NumericalPoint(2, -10.0),
                   NumericalPoint(2, 10.0), Indices(2, 21))
    print("graph=", graph)
    graph.draw("NMFDraw2D.png")

    # Full interfaces
    f = NumericalMathFunction(["x0", "x1", "x2"], ["y0", "y1"], [
                              "x0 * sin(x1 + 2.0 * x2) - 2.0 * x1 * cos(3.0 * x0 - x2)", "x1 * cos(x2 + 2.0 * x1) + 2.0 * x0 * cos(3.0 * x1 - x0)"])
    centralPoint = NumericalPoint([1.0, -0.5, 1.5])
    # First output as a function of first input around central point
    graph1D = f.draw(0, 0, centralPoint, -5.0, 5.0, 32)
    print("graph1D=", graph1D)
    graph1D.draw("NMFDraw1DFull.png")
    # Second output as a function of second and third inputs around central
    # point
    graph2D = f.draw(1, 2, 1, centralPoint, NumericalPoint(
        2, -5.0), NumericalPoint(2, 5.0), Indices(2, 21))
    print("graph1D=", graph2D)
    graph2D.draw("NMFDraw2DFull.png")

except:
    import sys
    print("t_NumericalMathFunction_draw.py",
          sys.exc_info()[0], sys.exc_info()[1])

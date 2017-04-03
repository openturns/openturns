#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    # A 1D->1D field
    mesh = IntervalMesher([10]).build(Interval(-2.0, 2.0))
    function = SymbolicFunction("x", "x")
    field = Field(mesh, function(mesh.getVertices()))
    graph = field.draw()
    graph = field.drawMarginal(0, False)
    graph = field.drawMarginal(0, True)
    # A 2D->1D field
    mesh = IntervalMesher([10] * 2).build(Interval([-2.0] * 2, [2.0] * 2))
    function = SymbolicFunction(["x0", "x1"], ["x0 - x1"])
    field = Field(mesh, function(mesh.getVertices()))
    graph = field.draw()
    graph = field.drawMarginal(0, False)
    graph = field.drawMarginal(0, True)
    # A 2D->2D field
    function = SymbolicFunction(["x0", "x1"], ["x0", "x1"])
    field = Field(mesh, function(mesh.getVertices()))
    graph = field.draw()

except:
    import sys
    print("t_Field_draw.py", sys.exc_info()[0], sys.exc_info()[1])

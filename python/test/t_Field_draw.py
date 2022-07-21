#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# A 1D->1D field
mesh = ot.IntervalMesher([10]).build(ot.Interval(-2.0, 2.0))
function = ot.SymbolicFunction("x", "x")
field = ot.Field(mesh, function(mesh.getVertices()))
graph = field.draw()
graph = field.drawMarginal(0, False)
graph = field.drawMarginal(0, True)
# A 2D->1D field
mesh = ot.IntervalMesher([10] * 2).build(ot.Interval([-2.0] * 2, [2.0] * 2))
function = ot.SymbolicFunction(["x0", "x1"], ["x0 - x1"])
field = ot.Field(mesh, function(mesh.getVertices()))
graph = field.draw()
graph = field.drawMarginal(0, False)
graph = field.drawMarginal(0, True)
# A 2D->2D field
function = ot.SymbolicFunction(["x0", "x1"], ["x0", "x1"])
field = ot.Field(mesh, function(mesh.getVertices()))
graph = field.draw()

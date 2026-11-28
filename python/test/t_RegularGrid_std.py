#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# start and step
start = -2.0
step = 0.1
# Number of steps for the RegularGrid
n = 10
regularGrid = ot.RegularGrid(start, step, n)

print("regularGrid =", regularGrid)
print("Start time =", regularGrid.getStart())
print("End time =", regularGrid.getEnd())
print("Time step =", regularGrid.getStep())
print("Steps =", regularGrid.getN())

# Mesh/RegularGrid cross-type comparison must hold in both directions
mesh = ot.Mesh(regularGrid)
assert mesh == regularGrid, "Mesh == RegularGrid"
assert regularGrid == mesh, "RegularGrid == Mesh"
assert not (mesh != regularGrid), "Mesh != RegularGrid"
assert not (regularGrid != mesh), "RegularGrid != Mesh"
# same grids compare equal, different grids do not
assert regularGrid == ot.RegularGrid(start, step, n), "same grids"
assert not (regularGrid != ot.RegularGrid(start, step, n)), "same grids"
otherGrid = ot.RegularGrid(start, step, n + 1)
assert regularGrid != otherGrid, "different grids"
assert not (regularGrid == otherGrid), "different grids"
assert ot.Mesh(regularGrid) != otherGrid, "Mesh vs different RegularGrid"
assert otherGrid != ot.Mesh(regularGrid), "different RegularGrid vs Mesh"

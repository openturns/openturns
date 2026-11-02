#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import os

ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(0)

# Grid constructors
bounds = ot.Interval([0.0, 0.0], [1.0, 1.0])
subdivisions = [5, 5]
grid = ot.Grid(bounds, subdivisions)
print("grid=", grid)
assert grid.getBounds() == bounds, "wrong bounds"
assert list(grid.getSubdivisions()) == subdivisions, "wrong subdivisions"
assert grid.getBoundingBox() == bounds, "wrong bounding box"

# default constructor
default_grid = ot.Grid()
assert default_grid.getBounds().getDimension() == 2, "wrong default dimension"
assert list(default_grid.getSubdivisions()) == [5, 5], "wrong default subdivisions"

# accessors
bounds2 = ot.Interval([-1.0, -2.0], [3.0, 4.0])
grid.setBounds(bounds2)
assert grid.getBounds() == bounds2, "wrong bounds after set"
grid.setSubdivisions([10, 3])
assert list(grid.getSubdivisions()) == [10, 3], "wrong subdivisions after set"
assert grid.getBoundingBox() == bounds2, "wrong bbox after set"

# invalid bounds dimension
with ott.assert_raises(ValueError):
    ot.Grid(ot.Interval([0.0], [1.0]), [5, 5])
with ott.assert_raises(ValueError):
    grid.setBounds(ot.Interval([0.0, 0.0, 0.0], [1.0, 1.0, 1.0]))

# invalid subdivisions size
with ott.assert_raises(ValueError):
    ot.Grid(bounds, [5])
with ott.assert_raises(ValueError):
    grid.setSubdivisions([5, 5, 5])

# zero subdivisions must be rejected (division by zero in View)
with ott.assert_raises(TypeError):
    ot.Grid(bounds, [0, 5])
with ott.assert_raises(TypeError):
    ot.Grid(bounds, [5, 0])
with ott.assert_raises(TypeError):
    grid.setSubdivisions([0, 5])

# persistence via Study
fileName = "grid_test.xml"
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager(fileName))
study.add("grid", grid)
study.save()
study2 = ot.Study()
study2.setStorageManager(ot.XMLStorageManager(fileName))
study2.load()
grid2 = ot.Grid()
study2.fillObject("grid", grid2)
os.remove(fileName)
assert grid2.getBounds() == bounds2, "persistence failed"
assert list(grid2.getSubdivisions()) == [10, 3], "persistence failed"

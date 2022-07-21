#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# 1d
mesh = ot.IntervalMesher([10]).build(ot.Interval(-2.0, 2.0))
graph = mesh.draw()

# 2d
mesh = ot.IntervalMesher([10] * 2).build(ot.Interval([-2.0] * 2, [2.0] * 2))
graph = mesh.draw()

# 3d
mesh = ot.IntervalMesher([10] * 3).build(ot.Interval([-2.0] * 3, [2.0] * 3))
graph = mesh.draw()
graph = mesh.draw3D(False, 0.2, 0.5, 0.9)

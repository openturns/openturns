#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp

ot.TESTPREAMBLE()

# 2d triangulation of a convex polygon
polyline = [[3.0, 0.0], [2.0, 0.0], [2.0, 0.75], [2.5, 0.75], [3.0, 0.2]]
mesher = otexp.PolygonMesher()
print("mesher=", mesher)
triangulation = mesher.build(polyline)
print("triangulation=", repr(triangulation))
assert triangulation.getDimension() == 2
assert triangulation.getVertices() == polyline
assert len(triangulation.getSimplices()) == 3
assert triangulation.getSimplices()[0] == [1, 0, 4]
assert triangulation.getSimplices()[1] == [4, 3, 2]
assert triangulation.getSimplices()[2] == [2, 1, 4]
assert triangulation.isValid()

# 2d triangulation of a non-convex polygon
polyline = [[0.0, 0.0], [0.0, 3.0], [1.0, 2.0], [0.5, 0.5], [1.0, 0.5], [2.0, 2.0], [2.0, 0.0]]
triangulation = mesher.build(polyline)
print("triangulation=", repr(triangulation))
assert triangulation.getDimension() == 2
assert triangulation.getVertices() == polyline
assert len(triangulation.getSimplices()) == 5
assert triangulation.getSimplices()[0] == [6, 5, 4]
assert triangulation.getSimplices()[1] == [3, 2, 1]
assert triangulation.getSimplices()[2] == [0, 6, 4]
assert triangulation.getSimplices()[3] == [3, 1, 0]
assert triangulation.getSimplices()[4] == [0, 4, 3]
assert triangulation.isValid()

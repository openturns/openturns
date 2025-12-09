#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp

ot.TESTPREAMBLE()

# 2d hull
vertices = [[3.0, 0.0], [2.0, 0.0], [2.0, 0.75,], [2.5, 0.75], [3.0, 0.2]]
mesher = otexp.ConvexHullMesher()
print("mesher=", mesher)
hull = mesher.build(vertices)
print("hull=", repr(hull))
assert hull.getDimension() == 2
assert len(hull.getSimplices()) == 5
assert hull.getIntrinsicDimension() == 1

# 3d hull of the unit cube
vertices = [[0, 0, 0], [0, 0, 1], [0, 1, 0], [0, 1, 1],
            [1, 0, 0], [1, 0, 1], [1, 1, 0], [1, 1, 1]]
hull = mesher.build(vertices)
print("hull=", repr(hull))
assert hull.getDimension() == 3
assert len(hull.getSimplices()) == 12
assert hull.isValid()
assert hull.getIntrinsicDimension() == 2

# nd hull of a random sample
for d in range(2, 8):
    vertices = ot.Normal(d).getSample(20)
    hull = mesher.build(vertices)
    print(f"d={d} hull={repr(hull)[:2000]}")
    assert hull.getDimension() == d
    assert hull.getIntrinsicDimension() == d - 1

#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
from math import exp, log

ot.TESTPREAMBLE()

inputVar = ["x", "y", "z"]
formula = ["0.05+0.95*x", "y-0.1*x*sin(x)", "z+0.1*x*sin(x)"]

for dim in [1, 2, 3]:
    interval = ot.Interval([0.0] * dim, [10.0] * dim)
    nrIntervals = int(exp(log(10000) / dim))
    mesh = ot.IntervalMesher([nrIntervals] * dim).build(interval)
    f = ot.SymbolicFunction(inputVar[0:dim], formula[0:dim])
    mesh.setVertices(f(mesh.getVertices()))

    simplices = mesh.getSimplices()
    nrSimplices = len(simplices)

    bvh = ot.BoundingVolumeHierarchy(mesh.getVertices(), simplices, 3)
    print("bvh=", bvh)

    ot.RandomGenerator.SetSeed(0)
    test = ot.ComposedDistribution(
        [ot.Uniform(-1.0, 11.0)] * dim).getSample(100)

    for i, vertex in enumerate(test):
        index = bvh.query(vertex)
        if index >= nrSimplices:
            print(i, "is outside")
        else:
            found, coordinates = mesh.checkPointInSimplexWithCoordinates(
                vertex, index)
            if not found:
                print("Wrong simplex found for", vertex,
                      "(index=", index, simplices[index], "barycentric coordinates=", coordinates)

    indices = bvh.query(test)
    for i, index in enumerate(indices):
        if index >= nrSimplices:
            print(i, "is outside")
        else:
            found, coordinates = mesh.checkPointInSimplexWithCoordinates(
                test[i], index)
            if not found:
                print("Wrong simplex found for", test[
                      i], "(index=", index, simplices[index], "barycentric coordinates=", coordinates)

# segfault with 1 simplex
mesh = ot.Mesh([[0.0, 0.0], [1.0, 0.0], [0.5, 1.0]], [[0, 1, 2]])
bvh = ot.BoundingVolumeHierarchy(mesh.getVertices(), mesh.getSimplices())
print(bvh.query([0.125, 0.2]))
print(bvh.query([0.125, 0.3]))

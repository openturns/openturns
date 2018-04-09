#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()

dim = 2
interval = ot.Interval([0.0] * dim, [10.0] * dim)
mesh = ot.IntervalMesher([30] * dim).build(interval)
f = ot.SymbolicFunction(["x", "y"], ["x + 0.5*sin(y)", "y-0.1*x*sin(x)"])
mesh.setVertices(f(mesh.getVertices()))

simplices = mesh.getSimplices()
nrSimplices = len(simplices)
naive = ot.NaiveEnclosingSimplex(mesh.getVertices(), simplices)
print("naive=", naive)

ot.RandomGenerator.SetSeed(0)
test = ot.ComposedDistribution([ot.Uniform(-1.0, 11.0)] * dim).getSample(100)

for i, vertex in enumerate(test):
    index = naive.query(vertex)
    if index >= nrSimplices:
        print(i, "is outside")
    else:
        found, coordinates = mesh.checkPointInSimplexWithCoordinates(
            vertex, index)
        if not found:
            print("Wrong simplex found for", vertex,
                  "(index=", index, simplices[index], "barycentric coordinates=", coordinates)

indices = naive.query(test)
for i, index in enumerate(indices):
    if index >= nrSimplices:
        print(i, "is outside")
    else:
        found, coordinates = mesh.checkPointInSimplexWithCoordinates(
            test[i], index)
        if not found:
            print("Wrong simplex found for", test[
                  i], "(index=", index, simplices[index], "barycentric coordinates=", coordinates)

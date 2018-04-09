#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import os

ot.TESTPREAMBLE()

nrVertices = 100
vertices = ot.Normal().getSample(nrVertices).sort()
simplices = [[i, i + 1] for i in range(nrVertices - 1)]
mesh1 = ot.Mesh(vertices, simplices)
vertices *= -1.0
mesh2 = ot.Mesh(vertices, simplices)

for mesh in [mesh1, mesh2]:
    lowerBound = mesh.getLowerBound()[0]
    upperBound = mesh.getUpperBound()[0]
    n = mesh.getSimplicesNumber()
    print("mesh=", mesh, "lowerBound=", lowerBound,
          "upperBound=", upperBound, n, "simplices")
    algo = ot.EnclosingSimplexMonotonic1D(mesh.getVertices())

    ot.RandomGenerator.SetSeed(0)
    test = ot.Sample(ot.Uniform(-3.0, 3.0).getSample(1000))

    vertices = mesh.getVertices()
    for vertex in test:
        index = algo.query(vertex)
        x = vertex[0]
        if x < lowerBound or x > upperBound:
            if index < n:
                print(
                    "Point", x, "should be outside but query returned index", index)
                os.exit(1)
        else:
            if index >= n:
                print(
                    "Point", x, "should be inside but query returned index", index)
                os.exit(1)

            found, coordinates = mesh.checkPointInSimplexWithCoordinates(
                vertex, index)
            if not found:
                print("Wrong simplex found for", x,
                      "(index=", index, ") barycentric coordinates=", coordinates)
                os.exit(1)
            if coordinates[0] < 0.0 or coordinates[0] > 1.0 or coordinates[1] < 0.0 or coordinates[1] > 1.0:
                print("Wrong barycentric coordinates found found for", x,
                      "(index=", index, ") barycentric coordinates=", coordinates)
                os.exit(1)
            if abs(x - coordinates[0] * vertices[index, 0] - coordinates[1] * vertices[index + 1, 0]) > 1.e-10:
                print("Wrong barycentric coordinates found found for", x,
                      "(index=", index, ") barycentric coordinates=", coordinates)
                os.exit(1)

#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import os

ot.TESTPREAMBLE()

grids = [ot.RegularGrid(1.0, 0.1, 20), ot.RegularGrid(-3.0, 0.1, 20),
         ot.RegularGrid(3.0, -0.1, 20), ot.RegularGrid(-1.0, -0.1, 20),
         ot.RegularGrid(-1.0, 0.13, 20), ot.RegularGrid(1.0, -0.13, 20)]

for regularGrid in grids:
    lowerBound = regularGrid.getLowerBound()[0]
    upperBound = regularGrid.getUpperBound()[0]
    n = regularGrid.getSimplicesNumber()
    print("regularGrid=", regularGrid, "lowerBound=",
          lowerBound, "upperBound=", upperBound, n, "simplices")
    algo = ot.RegularGridEnclosingSimplex(regularGrid)

    ot.RandomGenerator.SetSeed(0)
    test = ot.Sample(ot.Uniform(lowerBound - 0.2 * (upperBound - lowerBound),
                                upperBound + 0.2 * (upperBound - lowerBound)).getSample(1000))

    vertices = regularGrid.getVertices()
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

            found, coordinates = regularGrid.checkPointInSimplexWithCoordinates(
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

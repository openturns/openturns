#! /usr/bin/env python

import openturns as ot
import os
import openturns.testing as ott

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
    print(
        "mesh=",
        mesh,
        "lowerBound=",
        lowerBound,
        "upperBound=",
        upperBound,
        n,
        "simplices",
    )
    algo = ot.EnclosingSimplexMonotonic1D(mesh.getVertices())

    ot.RandomGenerator.SetSeed(0)
    test = ot.Sample(ot.Uniform(-3.0, 3.0).getSample(1000))

    vertices = mesh.getVertices()
    for vertex in test:
        index = algo.query(vertex)
        x = vertex[0]
        if x < lowerBound or x > upperBound:
            if index < n:
                print("Point", x, "should be outside but query returned index", index)
                os.exit(1)
        else:
            if index >= n:
                print("Point", x, "should be inside but query returned index", index)
                os.exit(1)

            found, coordinates = mesh.checkPointInSimplexWithCoordinates(vertex, index)
            if not found:
                print(
                    "Wrong simplex found for",
                    x,
                    "(index=",
                    index,
                    ") barycentric coordinates=",
                    coordinates,
                )
                os.exit(1)
            if (
                coordinates[0] < 0.0
                or coordinates[0] > 1.0
                or coordinates[1] < 0.0
                or coordinates[1] > 1.0
            ):
                print(
                    "Wrong barycentric coordinates found found for",
                    x,
                    "(index=",
                    index,
                    ") barycentric coordinates=",
                    coordinates,
                )
                os.exit(1)
            if (
                abs(
                    x
                    - coordinates[0] * vertices[index, 0]
                    - coordinates[1] * vertices[index + 1, 0]
                )
                > 1.0e-10
            ):
                print(
                    "Wrong barycentric coordinates found found for",
                    x,
                    "(index=",
                    index,
                    ") barycentric coordinates=",
                    coordinates,
                )
                os.exit(1)

# Extra coverage for error paths and accessors
locator = ot.EnclosingSimplexMonotonic1D(ot.Sample.BuildFromPoint([0.0, 1.0, 2.0]))
assert len(repr(locator)) > 0
assert len(str(locator)) > 0
assert locator.getVertices().getSize() == 3
# exact vertex hit returns the vertex index
assert locator.query([1.0]) == 1
# vector version
ott.assert_almost_equal(locator.queryScalar(ot.Point([0.5, 1.5, 5.0])), [0, 1, 2])
# out-of-range queries return simplicesNumber
assert locator.queryScalar(5.0) == 2
assert locator.queryScalar(-1.0) == 2
decreasing = ot.EnclosingSimplexMonotonic1D(
    ot.Sample.BuildFromPoint([2.0, 1.0, 0.0])
)
assert decreasing.queryScalar(-1.0) == 2
assert decreasing.queryScalar(5.0) == 2
assert decreasing.query([1.0]) == 1
# wrong point dimension
with ott.assert_raises(Exception):
    locator.query([1.0, 2.0])
# too few vertices
with ott.assert_raises(Exception):
    ot.EnclosingSimplexMonotonic1D(ot.Sample([[0.0]]))
# inconsistent simplices
with ott.assert_raises(Exception):
    bad = ot.EnclosingSimplexMonotonic1D()
    bad.setVerticesAndSimplices(ot.Sample.BuildFromPoint([0.0, 1.0]), [[1, 0]])
# non-monotonic vertices
with ott.assert_raises(Exception):
    ot.EnclosingSimplexMonotonic1D(ot.Sample([[0.0], [2.0], [1.0]]))
# save / load roundtrip
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("esm_test.xml"))
study.add("locator", locator)
study.save()
study2 = ot.Study()
study2.setStorageManager(ot.XMLStorageManager("esm_test.xml"))
study2.load()
loaded = ot.EnclosingSimplexMonotonic1D()
study2.fillObject("locator", loaded)
assert loaded.query([0.5]) == locator.query([0.5])
os.remove("esm_test.xml")

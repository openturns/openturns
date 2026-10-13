#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import os

ot.TESTPREAMBLE()

# Generate the data for the polygons to be drawn
size = 50
cursor = ot.Point(2)

data1 = ot.Sample(size, 2)  # polygon y = 2x for x in [-25]
data2 = ot.Sample(size, 2)  # polygon y = x*x for x in [-11]

for i in range(size):
    tmp = 7.0 * i / size + 2
    cursor[0] = tmp
    cursor[1] = 2 * tmp
    data1[i] = cursor

    tmp = 9.0 * i / size + 1
    cursor[0] = tmp
    cursor[1] = tmp * tmp
    data2[i] = cursor

# Create an empty graph
graph = ot.Graph("Some polygons", "x1", "x2")
graph.setLegendPosition("upper right")

# Create the first polygon
polygon1 = ot.Polygon(data1)
polygon1.setColor("blue")

# Then, draw it
graph.add(polygon1)

# Create the second polygon
polygon2 = ot.Polygon(data2)
polygon2.setColor("red")

# Add it to the graph
graph.add(polygon2)

# Fill below the start of the second polygon
polygon = ot.Polygon.FillBetween(data2[:10, 0], data2[:10, 1], [[data2[0, 1]]] * 10)

polygon.setColor("green")
graph.add(polygon)

# Try all scale types
for i in range(4):
    graph.setLogScale(i)

print("graph = ", graph)

# Extra coverage: constructors, edge color, FillBetween, persistence

defaultPolygon = ot.Polygon("default legend")
assert len(repr(defaultPolygon)) > 0
assert len(str(defaultPolygon)) > 0
samplePolygon = ot.Polygon(data1, "sample legend")
assert samplePolygon.getEdgeColor() == samplePolygon.getColor()
xyPolygon = ot.Polygon(data1[:, 0], data1[:, 1], "xy legend")
ott.assert_almost_equal(xyPolygon.getData(), data1, 1e-14, 1e-14)
pointPolygon = ot.Polygon(ot.Point([0.0, 1.0]), ot.Point([2.0, 3.0]), "point legend")
ott.assert_almost_equal(
    pointPolygon.getData(), [[0.0, 2.0], [1.0, 3.0]], 1e-14, 1e-14
)
oneDPolygon = ot.Polygon(ot.Sample([[1.0], [2.0], [3.0]]), "red", "blue", "1d legend")
ott.assert_almost_equal(
    oneDPolygon.getData(), [[0.0, 1.0], [1.0, 2.0], [2.0, 3.0]], 1e-14, 1e-14
)
assert oneDPolygon.getEdgeColor() == "blue"
clonePolygon = ot.Polygon(polygon1)
ott.assert_almost_equal(clonePolygon.getData(), polygon1.getData(), 1e-14, 1e-14)
with ott.assert_raises(Exception):
    ot.Polygon(ot.Sample([[0.0], [1.0]]), ot.Sample([[0.0]]))
with ott.assert_raises(Exception):
    ot.Polygon(ot.Point([0.0]), ot.Point([0.0, 1.0]))
with ott.assert_raises(Exception):
    ot.Polygon(ot.Sample([[1.0, 2.0, 3.0]]))
with ott.assert_raises(Exception):
    polygon1.setEdgeColor("invalid-color")
fillPoint = ot.Polygon.FillBetween(
    ot.Point([0.0, 1.0]), ot.Point([0.0, 1.0]), ot.Point([1.0, 2.0])
)
assert fillPoint.getData().getSize() == 4
with ott.assert_raises(Exception):
    ot.Polygon.FillBetween(ot.Point([0.0]), ot.Point([0.0, 1.0]), ot.Point([0.0]))
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("t_Polygon_std_tmp.xml"))
study.add("poly", polygon1)
study.save()
loadedStudy = ot.Study()
loadedStudy.setStorageManager(ot.XMLStorageManager("t_Polygon_std_tmp.xml"))
loadedStudy.load()
loadedPoly = ot.Polygon()
loadedStudy.fillObject("poly", loadedPoly)
assert loadedPoly.getEdgeColor() == polygon1.getEdgeColor()
os.remove("t_Polygon_std_tmp.xml")

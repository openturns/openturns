#! /usr/bin/env python

import openturns as ot

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
graph = ot.Graph("Some polygons", "x1", "x2", True, "topright")

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

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
myGraph = ot.Graph("Some polygons", "x1", "x2", True, "topright", 1.0)

# Create the first polygon
myPolygon1 = ot.Polygon(data1)
myPolygon1.setColor("blue")

# Then, draw it
myGraph.add(myPolygon1)

# Create the second polygon
myPolygon2 = ot.Polygon(data2)
myPolygon2.setColor("red")

# Add it to the graph
myGraph.add(myPolygon2)

# Fill below the start of the second polygon
polygon = ot.Polygon.FillBetween([data2[i][0] for i in range(10)],
                                 [data2[i][1] for i in range(10)],
                                 [data2[0][1]] * 10)
polygon.setColor("green")
myGraph.add(polygon)

# Try all scale types
for i in range(4):
    myGraph.setLogScale(i)

print("myGraph = ", myGraph)

#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(0)

# Generate the data for the polygonArrays to be drawn
generator = ot.Normal(2)
size = 5
array = []
palette = ot.Drawable.BuildDefaultPalette(size)
for i in range(size):
    vertices = generator.getSample(3)
    array.append(ot.Polygon(vertices, palette[i], palette[size - i - 1]))

graph = ot.Graph('An array of polygons', 'x', 'y', True, '')
graph.add(ot.PolygonArray(array))

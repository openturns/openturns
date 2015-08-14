import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View

# Generate the data for the polygons to be drawn
size = 50
cursor = ot.NumericalPoint(2)

data1 = ot.NumericalSample(size, 2)  # polygon y = 2x for x in [-25]
data2 = ot.NumericalSample(size, 2)  # polygon y = x*x for x in [-11]

for i in range(size):
    tmp = 7. * i / size + 2
    cursor[0] = tmp
    cursor[1] = 2 * tmp
    data1[i] = cursor

    tmp = 9. * i / size + 1
    cursor[0] = tmp
    cursor[1] = tmp * tmp
    data2[i] = cursor

# Create an empty graph
graph = ot.Graph("Polygons example", "x1", "x2", True, "", 1.0)

# Create the first polygon
myPolygon1 = ot.Polygon(data1)
myPolygon1.setColor("blue")

# Then, draw it
graph.add(myPolygon1)

# Create the second polygon
myPolygon2 = ot.Polygon(data2)
myPolygon2.setColor("red")

# Add it to the graph and draw everything
graph.add(myPolygon2)
graph.setLogScale(1)

fig = plt.figure(figsize=(5, 5))
View(graph, figure=fig)

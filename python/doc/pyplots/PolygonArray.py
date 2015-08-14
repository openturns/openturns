import openturns as ot
from openturns.viewer import View

ot.RandomGenerator.SetSeed(0)
generator = ot.Normal(2)
size = 5
array = []
palette = ot.Drawable.BuildDefaultPalette(size)
for i in range(size):
    vertices = generator.getSample(3)
    array.append(ot.Polygon(vertices, palette[i], palette[size - i - 1]))

graph = ot.Graph('PolygonArray example', 'x1', 'x2', True, '', 1.0)
graph.add(ot.PolygonArray(array))

View(graph, figure_kwargs={'figsize': (4, 4)})

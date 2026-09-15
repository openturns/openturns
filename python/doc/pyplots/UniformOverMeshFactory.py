import openturns as ot
import openturns.experimental as otexp
from matplotlib import pyplot as plt
import openturns.viewer as otv

ot.RandomGenerator.SetSeed(0)

vertices = ot.Sample(0, 3)
simplices = []
grid = 6
for j in range(grid + 1):
    for i in range(grid + 1):
        vertices.add([i / grid, j / grid, 0.0])
for j in range(grid):
    for i in range(grid):
        index = j * (grid + 1) + i
        simplices.append([index, index + 1, index + grid + 1])
        simplices.append([index + 1, index + grid + 2, index + grid + 1])
mesh = ot.Mesh(vertices, ot.IndicesCollection(simplices))
sample = ot.UniformOverMesh(mesh).getSample(500)

factory = otexp.UniformOverMeshFactory()
estimated = factory.buildAsUniformOverMesh(sample)
estimatedMesh = estimated.getMesh()

graph = estimatedMesh.draw()
graph.setTitle("Mesh estimated from the sample")
graph.setXTitle("x")
graph.setYTitle("y")

fig = plt.figure(figsize=(10, 10))
axis = fig.add_subplot(111)
otv.View(graph, figure=fig, axes=[axis], add_legend=True, square_axes=True)
fig.suptitle("UniformOverMeshFactory")

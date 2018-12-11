import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View

# Generate sample with the given plane
levels = [4, 4]
myPlane = ot.Box(levels)
sample = myPlane.generate()

# Create an empty graph
graph = ot.Graph("Box plane", "x1", "x2", True, "")

# Create the cloud
cloud = ot.Cloud(sample, "blue", "fsquare", "")

# Then, draw it
graph.add(cloud)

fig = plt.figure(figsize=(4, 4))
axis = fig.add_subplot(111)
axis.set_xlim(auto=True)
View(graph, figure=fig, axes=[axis], add_legend=False)

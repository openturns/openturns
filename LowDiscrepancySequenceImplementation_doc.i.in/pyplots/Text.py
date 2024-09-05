import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View

distribution = ot.Normal(2)

sample = distribution.getSample(30)

# Create an empty graph
graph = ot.Graph("Text example", "x1", "x2", True, "")

# Create the cloud
cloud = ot.Cloud(sample, "blue", "fsquare", "")

# Then, draw it
graph.add(cloud)

# Display extrema indices
x1 = [x[0] for x in sample[:, 0]]
x2 = [x[0] for x in sample[:, 1]]
idx = [0] * 4
idx[0] = x1.index(min(x1))
idx[1] = x1.index(max(x1))
idx[2] = x2.index(min(x2))
idx[3] = x2.index(max(x2))

labels = ot.Description(sample.getSize())
for i in range(4):
    labels[idx[i]] = str(idx[i])

position = ot.Description(sample.getSize(), "top")
position[idx[0]] = "right"
position[idx[1]] = "left"
position[idx[2]] = "top"
position[idx[3]] = "bottom"

annotations = ot.Text(sample, labels)
annotations.setColor("red")
annotations.setTextPositions(position)

graph.add(annotations)

fig = plt.figure(figsize=(4, 4))
axis = fig.add_subplot(111)
axis.set_xlim(auto=True)
View(graph, figure=fig, axes=[axis], add_legend=False)

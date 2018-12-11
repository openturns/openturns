import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View

ot.RandomGenerator.SetSeed(0)
R = ot.CorrelationMatrix(2)
for i in range(1, 2):
    R[i, i - 1] = -0.25

distribution = ot.Normal([-1.5, 0.5], [4., 1.], R)

sample = distribution.getSample(200)

# Create an empty graph
graph = ot.Graph("Cloud example", "x1", "x2", True, "")

# Create the cloud
cloud = ot.Cloud(sample, "blue", "fsquare", "")

# Then, draw it
graph.add(cloud)

fig = plt.figure(figsize=(4, 4))
axis = fig.add_subplot(111)
axis.set_xlim(auto=True)
View(graph, figure=fig, axes=[axis], add_legend=False)

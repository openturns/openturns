import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View

R = ot.CorrelationMatrix(3)
for i in range(1, 3):
    R[i, i - 1] = -0.4

distribution = ot.Normal([0.] * 3, [5.] * 3, R)

sample = distribution.getSample(200)
# Create an empty graph
graph = ot.Graph("Pairs example", " ", " ", True, "")

# Create the Pairs
pairs = ot.Pairs(sample, "Pairs example",
                 sample.getDescription(), "blue", "bullet")

# Then, draw it
graph.add(pairs)

fig = plt.figure(figsize=(5, 4))
axis = fig.add_subplot(111)
axis.set_xlim(auto=True)
View(graph, figure=fig, axes=[axis], add_legend=False)

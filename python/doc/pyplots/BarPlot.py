import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View
from math import floor

ot.RandomGenerator.SetSeed(0)
distribution = ot.Normal([0.5], [2.0], ot.CorrelationMatrix(1))
sample = distribution.getSample(2000)

# Construct histogram
epsilon = 0.1
minSample = sample.getMin()[0]
maxSample = sample.getMax()[0] + 0.1
nBars = 20
data = ot.Sample(nBars, [(maxSample - minSample) / nBars, 0])

for i in range(2000):
    index = int(
        floor((sample[i, 0] - minSample) / (maxSample - minSample) * nBars))
    data[index, 1] += 1

# Create an empty graph
graph = ot.Graph(
    "Sample histogram example", "Realizations", "Frequency", True, "topright")

# Create the barplot
barPlot = ot.BarPlot(data, minSample, "Normal histogram")

# Then, draw it
graph.add(barPlot)

fig = plt.figure(figsize=(6, 4))
axis = fig.add_subplot(111)
axis.set_xlim(auto=True)
View(graph, figure=fig, axes=[axis], add_legend=True)

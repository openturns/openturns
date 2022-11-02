import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View

ot.RandomGenerator.SetSeed(0)

# Generate sample with the given plane
distribution = ot.ComposedDistribution(
    [ot.Exponential(), ot.Triangular(-1.0, -0.5, 1.0)]
)
marginalSizes = ot.Indices([3, 6])
experiment = ot.GaussProductExperiment(ot.Distribution(distribution), marginalSizes)

sample = experiment.generate()

# Create an empty graph
graph = ot.Graph("Gauss product experiment", "x1", "x2", True, "")

# Create the cloud
cloud = ot.Cloud(sample, "blue", "fsquare", "")

# Then, draw it
graph.add(cloud)

fig = plt.figure(figsize=(4, 4))
axis = fig.add_subplot(111)
axis.set_xlim(auto=True)
View(graph, figure=fig, axes=[axis], add_legend=False)

import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View
ot.RandomGenerator.SetSeed(0)

experiment1 = ot.GaussProductExperiment(ot.Uniform(0.0, 1.0), [3])
experiment2 = ot.GaussProductExperiment(ot.Uniform(0.0, 1.0), [5])
collection = [experiment1, experiment2]
experiment = ot.TensorProductExperiment(collection)
sample = experiment.generate()

# Create an empty graph
graph = ot.Graph("Tensor product Gauss experiment", "x1", "x2", True, "")

# Create the cloud
cloud = ot.Cloud(sample, "blue", "fsquare", "")

# Then, draw it
graph.add(cloud)

fig = plt.figure(figsize=(4, 4))
axis = fig.add_subplot(111)
axis.set_xlim(auto=True)
View(graph, figure=fig, axes=[axis], add_legend=False)

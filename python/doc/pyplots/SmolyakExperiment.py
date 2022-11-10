import openturns as ot
import openturns.experimental as otexp
from matplotlib import pyplot as plt
from openturns.viewer import View

ot.RandomGenerator.SetSeed(0)

experiment1 = ot.GaussProductExperiment(ot.Uniform(0.0, 1.0))
experiment2 = ot.GaussProductExperiment(ot.Uniform(0.0, 1.0))
collection = [experiment1, experiment2]
level = 3
smolyak = otexp.SmolyakExperiment(collection, level)

sample = smolyak.generate()

# Create an empty graph
graph = ot.Graph("Smolyak experiment", "x1", "x2", True, "")

# Create the cloud
cloud = ot.Cloud(sample, "blue", "fsquare", "")

# Then, draw it
graph.add(cloud)

fig = plt.figure(figsize=(4, 4))
axis = fig.add_subplot(111)
axis.set_xlim(auto=True)
View(graph, figure=fig, axes=[axis], add_legend=False)

from __future__ import print_function
import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View
import math as m

Id = ot.IdentityMatrix(2)
atoms = [ot.Normal([1.0, 2.0], [0.5, 0.8], Id), ot.Normal(
    [1.0, -2.0], [0.9, 0.8], Id), ot.Normal([-1.0, 0.0], [0.5, 0.6], Id)]
weights = [0.3, 0.3, 0.4]
mixture = ot.Mixture(atoms, weights)
data = mixture.getSample(1000)
classifier = ot.MixtureClassifier(mixture)
graph = mixture.drawPDF(data.getMin(), data.getMax())
graph.setLegendPosition("")
graph.setTitle("MixtureClassifier example")
classes = classifier.classify(data)
palette = ot.Drawable.BuildDefaultPalette(len(atoms))
symbols = ot.Drawable.GetValidPointStyles()
for i in range(classes.getSize()):
    index = classes[i]
    graph.add(
        ot.Cloud([data[i]], palette[index % len(palette)], symbols[index % len(symbols)]))

fig = plt.figure(figsize=(4, 4))
axis = fig.add_subplot(111)
axis.set_xlim(auto=True)
View(graph, figure=fig, axes=[axis], add_legend=False)

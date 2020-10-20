import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View

ot.RandomGenerator.SetSeed(0)

graph = ot.Normal().drawPDF()

fig = plt.figure()
View(graph, figure=fig, add_legend=False, plot_kw={'color': 'blue'})

import openturns as ot
from matplotlib import pyplot as plt
import openturns.viewer as otv

ot.RandomGenerator.SetSeed(0)

graph = ot.Normal().drawPDF()

fig = plt.figure()
otv.View(graph, figure=fig, add_legend=False, plot_kw={"color": "blue"})

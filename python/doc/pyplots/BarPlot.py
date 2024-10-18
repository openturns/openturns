import openturns as ot
from openturns.viewer import View

graph = ot.Graph('BarPlot', 'X', 'Y', True, 'topright')
origin = 0.0
data = [[1.0, 2.0], [3.0, 4.0], [2.0, 3.0]]
barPlot = ot.BarPlot(data, origin, 'orange', 'shaded', 'dashed', 'label')
graph.add(barPlot)

View(graph, figure_kw={"figsize": (6, 4)})

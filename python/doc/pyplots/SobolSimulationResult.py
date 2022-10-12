import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View

fo_dist = ot.Normal(
    [0.154081, 0.872087, 0.0122877, 0.0433801],
    [0.00982468, 0.0302524, 0.00911937, 0.00926643],
    ot.CorrelationMatrix(4),
)
to_dist = ot.Normal(
    [0.16743, 0.795477, 0.0124454, 0.045178],
    [0.0068919, 0.00950876, 0.00204324, 0.0038844],
    ot.CorrelationMatrix(4),
)
result = ot.SobolSimulationResult(fo_dist, to_dist, 250, 100)

graph = result.draw()

fig = plt.figure(figsize=(8, 4))
axis = fig.add_subplot(111)
axis.set_xlim(auto=True)
View(graph, figure=fig, axes=[axis], add_legend=True)

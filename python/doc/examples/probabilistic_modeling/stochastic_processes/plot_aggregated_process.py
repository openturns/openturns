"""
Aggregate processes
===================
"""

# %%
# In this example we are going to concatenate several processes that share the same mesh.

# %%
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pyplot as plt


# %%
# Create processes to aggregate
myMesher = ot.IntervalMesher([100, 10])
lowerbound = [0.0, 0.0]
upperBound = [2.0, 4.0]
myInterval = ot.Interval(lowerbound, upperBound)
myMesh = myMesher.build(myInterval)
myProcess1 = ot.WhiteNoise(ot.Normal(), myMesh)
myProcess2 = ot.WhiteNoise(ot.Triangular(), myMesh)


# %%
# Draw values of a realization of the 2nd process
marginal = ot.HistogramFactory().build(myProcess1.getRealization().getValues())
graph = marginal.drawPDF()
view = viewer.View(graph)

# %%
# Create an aggregated process
myAggregatedProcess = ot.AggregatedProcess([myProcess1, myProcess2])

# %%
# Draw values of the realization on the 2nd marginal
marginal = ot.HistogramFactory().build(
    myAggregatedProcess.getRealization().getValues().getMarginal(0)
)
graph = marginal.drawPDF()
viewer.View(graph)
plt.show()

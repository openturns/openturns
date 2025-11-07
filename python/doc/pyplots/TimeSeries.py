import openturns as ot
from matplotlib import pyplot as plt
import openturns.viewer as otv

ot.RandomGenerator.SetSeed(0)
# Create a bivariate Gaussian process
myTG = ot.RegularGrid(0.0, 0.01, 100)
myCov = ot.ExponentialModel([3.0], [1.2])
myProcess = ot.GaussianProcess(myCov, myTG)

myTS = myProcess.getRealization()

graph = myTS.drawMarginal(0)
graph.setTitle("A time series")

fig = plt.figure(figsize=(8, 4))
axis = fig.add_subplot(111)
axis.set_xlim(auto=True)

otv.View(graph, figure=fig, axes=[axis], add_legend=True)

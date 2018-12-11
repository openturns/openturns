import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View


# Create a bivariate Gaussian process
myTG = ot.RegularGrid(0.0, 0.01, 100)
myCov = ot.ExponentialModel([3.0], [1.2])
myProcess = ot.GaussianProcess(myCov, myTG)

myPS = myProcess.getSample(6)


sample_graph = myPS.drawMarginal(0)
sample_graph.setTitle('A sample of fields')

fig = plt.figure(figsize=(10, 4))
sample_axis = fig.add_subplot(111)
View(sample_graph, figure=fig, axes=[sample_axis], add_legend=False)

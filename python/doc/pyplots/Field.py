import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View


# Create a bivariate normal process
myMesh = ot.IntervalMesher([100, 100]).build(ot.Interval([0.0, 0.0], [1.0, 1.0]))
myCov = ot.ExponentialModel(2, [1.2, 2.0], [3.0, 4.0])
myProcess = ot.TemporalNormalProcess(myCov, myMesh)

myField = myProcess.getRealization()

graph = myField.draw()

fig = plt.figure(figsize=(8, 4))
plt.suptitle("A field")
axis = fig.add_subplot(111)
axis.set_xlim(auto=True)

View(graph, figure=fig, axes=[axis], add_legend=True)

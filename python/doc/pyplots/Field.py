import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View


# Create a bivariate normal process
myMesh = ot.IntervalMesher([39, 39]).build(ot.Interval([0.0] * 2, [1.0] * 2))
myCov = ot.GeneralizedExponential(2 * [0.1], 1.3)
myProcess = ot.TemporalNormalProcess(myCov, myMesh)

myField = myProcess.getRealization()

graph = myField.drawMarginal(0, False)

fig = plt.figure(figsize=(8, 4))
plt.suptitle("A field")
axis = fig.add_subplot(111)
axis.set_xlim(auto=True)
View(graph, figure=fig, axes=[axis], add_legend=True)

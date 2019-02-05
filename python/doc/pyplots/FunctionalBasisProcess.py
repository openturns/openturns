import openturns as ot
from math import exp
from matplotlib import pyplot as plt
from openturns.viewer import View

f1 = ot.SymbolicFunction(['t'], ['sin(t)'])
f2 = ot.SymbolicFunction(['t'], ['cos(t)*cos(t)'])
myBasis = ot.Basis([f1, f2])
coefDis = ot.Normal([2] * 2, [5] * 2, ot.CorrelationMatrix(2))
myTG = ot.RegularGrid(0.0, 0.1, 250)

myFBP = ot.FunctionalBasisProcess(coefDis, myBasis, myTG)

TS = myFBP.getRealization()


graph = TS.draw()
graph.add(myFBP.getRealization().draw())
graph.add(myFBP.getRealization().draw())
graph.setColors(['red', 'blue', 'green'])
graph.setTitle('Functional Basis Process')

fig = plt.figure(figsize=(10, 4))
fbp_axis = fig.add_subplot(111)
view = View(graph, figure=fig, axes=[fbp_axis], add_legend=False)

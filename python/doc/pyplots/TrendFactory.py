import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View

# Create a process on a regular grid
myGrid = ot.RegularGrid(0.0, 0.1, 100)

amplitude = [5.0]
scale = [0.2]
myCovModel = ot.ExponentialModel(scale, amplitude)
myXProcess = ot.GaussianProcess(myCovModel, myGrid)

# Create a trend
fTrend = ot.SymbolicFunction(["t"], ["1+2*t+t^2"])
fTemp = ot.TrendTransform(fTrend, myGrid)

# Add the trend to the process and get a field
myYProcess = ot.CompositeProcess(fTemp, myXProcess)
myYField = myYProcess.getRealization()

# Create a TrendFactory
myBasisSequenceFactory = ot.LARS()
myFittingAlgorithm = ot.KFold()
func1 = ot.SymbolicFunction(["t"], ["1"])
func2 = ot.SymbolicFunction(["t"], ["t"])
func3 = ot.SymbolicFunction(["t"], ["t^2"])
myBasis = ot.Basis([func1, func2, func3])

myTrendFactory = ot.TrendFactory(myBasisSequenceFactory, myFittingAlgorithm)

# Estimate the trend
myTrendTransform = myTrendFactory.build(myYField, myBasis)


graph = myTrendTransform.getTrendFunction().draw(0.0, 10.0)
graph.add(fTrend.draw(0.0, 10.0))
graph.add(ot.Cloud(myYField.getMesh().getVertices(), myYField.getValues()))
graph.setColors(['red', 'blue', 'black'])
graph.setLegends(['estimated trend', 'real trend', 'sample'])
graph.setLegendPosition('topleft')
graph.setYTitle('values')
graph.setTitle("Trend estimation from a field")

fig = plt.figure(figsize=(8, 4))
axis = fig.add_subplot(111)
axis.set_xlim(auto=True)

View(graph, figure=fig, axes=[axis], add_legend=True)

import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View

# Create a process X: R^2 --> R^2

# Define a bi dimensional mesh as a box
myIndices = ot.Indices([40, 20])
myMesher = ot.IntervalMesher(myIndices)
lowerBound = [0.0, 0.0]
upperBound = [2.0, 1.0]
myInterval = ot.Interval(lowerBound, upperBound)
myMesh = myMesher.build(myInterval)


# Define a scalar temporal Gaussian process on the mesh
# this process is stationary
# myXproc R^2 --> R
amplitude = [1.0]
scale = [0.2, 0.2]
myCovModel = ot.ExponentialModel(scale, amplitude)
myXproc = ot.GaussianProcess(myCovModel, myMesh)


# Transform myXproc to make its variance depend on the vertex (s,t)
# and to get a positive process
# thanks to the spatial function g
# myXtProcess R --> R
g = ot.SymbolicFunction(['x1'],  ['exp(x1)'])
myDynTransform = ot.ValueFunction(g, myMesh)
myXtProcess = ot.CompositeProcess(myDynTransform, myXproc)

myField = myXtProcess.getRealization()
graphMarginal1 = ot.KernelSmoothing().build(myField.getValues()).drawPDF()
graphMarginal1.setTitle("")
graphMarginal1.setXTitle("X")
graphMarginal1.setLegendPosition("")

# Initiate a BoxCoxFactory
myBoxCoxFactory = ot.BoxCoxFactory()

graph = ot.Graph()
shift = [0.0]

# We estimate the lambda parameter from the field myField
# All values of the field are positive
myModelTransform = myBoxCoxFactory.build(myField, shift, graph)
graphMarginal2 = ot.KernelSmoothing().build(
    myModelTransform(myField).getValues()).drawPDF()
graphMarginal2.setXTitle("T_lambda(X)")
graphMarginal2.setLegendPosition("")

graph.setLegendPosition("bottomright")

fig = plt.figure(figsize=(12, 4))
axis = fig.add_subplot(131)
axis.set_xlim(auto=True)
View(graphMarginal1, figure=fig, axes=[axis], add_legend=True)
axis = fig.add_subplot(132)
axis.set_xlim(auto=True)
View(graph, figure=fig, axes=[axis], add_legend=True)
axis = fig.add_subplot(133)
axis.set_xlim(auto=True)
View(graphMarginal2, figure=fig, axes=[axis], add_legend=True)
fig.suptitle("Box Cox log-likelihood example")

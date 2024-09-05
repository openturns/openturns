import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View

# Create a process X: R^2 --> R^2

# Define a bi dimensional mesh as a box
indices = ot.Indices([40, 20])
mesher = ot.IntervalMesher(indices)
lowerBound = [0.0, 0.0]
upperBound = [2.0, 1.0]
interval = ot.Interval(lowerBound, upperBound)
mesh = mesher.build(interval)


# Define a scalar temporal Gaussian process on the mesh
# this process is stationary
# xProc R^2 --> R
amplitude = [1.0]
scale = [0.2, 0.2]
covModel = ot.ExponentialModel(scale, amplitude)
xProc = ot.GaussianProcess(covModel, mesh)


# Transform xProc to make its variance depend on the vertex (s,t)
# and to get a positive process
# thanks to the spatial function g
# xtProcess R --> R
g = ot.SymbolicFunction(["x1"], ["exp(x1)"])
dynTransform = ot.ValueFunction(g, mesh)
xtProcess = ot.CompositeProcess(dynTransform, xProc)

field = xtProcess.getRealization()
graphMarginal1 = ot.KernelSmoothing().build(field.getValues()).drawPDF()
graphMarginal1.setTitle("")
graphMarginal1.setXTitle("X")
graphMarginal1.setLegendPosition("")

# Initiate a BoxCoxFactory
factory = ot.BoxCoxFactory()

shift = [0.0]

# We estimate the lambda parameter from the field field
# All values of the field are positive
transform, graph = factory.buildWithGraph(field, shift)
graphMarginal2 = ot.KernelSmoothing().build(transform(field).getValues()).drawPDF()
graphMarginal2.setXTitle("T_lambda(X)")
graphMarginal2.setLegendPosition("")

graph.setLegendPosition("lower right")

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

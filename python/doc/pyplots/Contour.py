import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View

# Create a function
f = ot.SymbolicFunction(
    ["x", "y"], ["exp(-sin(cos(y)^2*x^2+sin(x)^2*y^2))"])

# Generate the data for the curves to be drawn
nX = 75
nY = 75
inputData = ot.Box([nX, nY]).generate()
inputData *= [10.0] * 2
inputData += [-5.0] * 2
data = f(inputData)
levels = [(0.5 + i) / 5 for i in range(5)]
# Create an empty graph
graph = ot.Graph("Complex iso lines example", "u1", "u2", True, "")

# Create the contour
contour = ot.Contour(nX + 2, nY + 2, data)
contour.setLevels(levels)

# Then, draw it
graph.add(contour)
fig = plt.figure(figsize=(4, 4))
axis = fig.add_subplot(111)
axis.set_xlim(auto=True)
View(graph, figure=fig, axes=[axis], add_legend=False)

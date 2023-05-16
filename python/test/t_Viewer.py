#! /usr/bin/env python

from openturns.viewer import View
import openturns as ot

# Curve
graph = ot.Normal().drawCDF()
# graph.draw('curve1.png')
view = View(graph, pixelsize=(800, 600), plot_kw={"color": "blue"})
# view.save('curve1.png')
view.show()

# Contour
graph = ot.Normal([1, 2], [3, 5], ot.CorrelationMatrix(2)).drawPDF()
# graph.draw('curve2.png')
view = View(graph)
# view.save('curve2.png')
view.show()

# Histogram tests
normal = ot.Normal(1)
size = 100
sample = normal.getSample(size)
graph = ot.HistogramFactory().build(sample, 10).drawPDF()
# graph.draw('curve3.png')
view = View(graph)
# view.save('curve3.png')
view.show()

# QQPlot tests
size = 100
normal = ot.Normal(1)
sample = normal.getSample(size)
sample2 = ot.Gamma(3.0, 4.0, 0.0).getSample(size)
graph = ot.VisualTest.DrawQQplot(sample, sample2, 100)
# graph.draw('curve4.png')
view = View(graph)
# view.save('curve4.png')
view.ShowAll(block=True)

# Clouds tests
dimension = 2
R = ot.CorrelationMatrix(dimension)
R[0, 1] = 0.8
distribution = ot.Normal(ot.Point(dimension, 3.0), ot.Point(dimension, 2.0), R)
size = 100
sample1 = ot.Normal([3.0] * dimension, [2.0] * dimension, R).getSample(size)
sample2 = ot.Normal([2.0] * dimension, [3.0] * dimension, R).getSample(size // 2)
cloud1 = ot.Cloud(sample1, "blue", "fsquare", "Sample1 Cloud")
cloud2 = ot.Cloud(sample2, "red", "fsquare", "Sample2 Cloud")
graph = ot.Graph("two samples clouds", "x1", "x2", True, "topright")
graph.add(cloud1)
graph.add(cloud2)
# graph.draw('curve5.png')
view = View(graph)
# view.save('curve5.png')
view.show()

# Text
graph = ot.Graph("Annotated cloud", "x", "y", True, "")
distribution = ot.Normal(2)
size = 30
sample2D = distribution.getSample(size)
cloud = ot.Cloud(sample2D, "red", "fsquare", "Sample2D Cloud")
graph.add(cloud)

# Display extrema indices
x1 = [x[0] for x in sample2D[:, 0]]
x2 = [x[0] for x in sample2D[:, 1]]
idx = [0] * 4
idx[0] = x1.index(min(x1))
idx[1] = x1.index(max(x1))
idx[2] = x2.index(min(x2))
idx[3] = x2.index(max(x2))

labels = ot.Description(sample2D.getSize())
for i in range(4):
    labels[idx[i]] = str(idx[i])

position = ot.Description(sample2D.getSize(), "top")
position[idx[0]] = "right"
position[idx[1]] = "left"
position[idx[2]] = "top"
position[idx[3]] = "bottom"

text = ot.Text(sample2D, labels)
text.setColor("red")
text.setTextPositions(position)

graph.add(text)
view = View(graph)
view.show()

# CobWeb tests
size = 100
dim = 6
inputSample = ot.Normal(dim).getSample(size)
inputVar = list(["X" + str(i) for i in range(dim)])
expression = ""
for i in range(dim):
    if i > 0:
        expression += "+"
    expression += "cos(" + str(i + 1) + "*" + inputVar[i] + ")"
model = ot.SymbolicFunction(inputVar, [expression])
outputSample = model(inputSample)
graph = ot.VisualTest.DrawParallelCoordinates(
    inputSample, outputSample, 2.5, 3.0, "red", False
)
# graph.draw('curve6.png')
view = View(graph, legend_kw={"loc": "lower center"})
# view.save('curve6.png')
view.show()

# Staircase
distribution = ot.Poisson(10.0)
graph = distribution.drawCDF()
# graph.draw('curve7.png')
view = View(graph)
# view.save('curve7.png')
view.ShowAll(block=True)

# Pie
graph = ot.SobolIndicesAlgorithm.DrawImportanceFactors(
    [0.4, 0.3, 0.2, 0.1], ["a0", "a1", "a2", "a3"], "Zou"
)
# graph.draw('curve8.png')
view = View(graph)
# view.save('curve8.png')
view.show()

# Convergence graph curve
aCollection = []
aCollection.append(
    ot.LogNormalFactory().build(ot.LogNormalMuSigma()([300.0, 30.0, 0.0]))
)
aCollection.append(ot.Normal(75e3, 5e3))
myDistribution = ot.ComposedDistribution(aCollection)
vect = ot.RandomVector(myDistribution)
LimitState = ot.SymbolicFunction(("R", "F"), ("R-F/(pi_*100.0)",))
G = ot.CompositeRandomVector(LimitState, vect)
myEvent = ot.ThresholdEvent(G, ot.Less(), 0.0)
experiment = ot.MonteCarloExperiment()
myAlgo = ot.ProbabilitySimulationAlgorithm(myEvent, experiment)
myAlgo.setMaximumCoefficientOfVariation(0.05)
myAlgo.setMaximumOuterSampling(int(1e5))
myAlgo.run()
graph = myAlgo.drawProbabilityConvergence()
# graph.draw('curve10.png')
view = View(graph)
# view.save('curve10.png')
view.show()

# Polygon
size = 50
cursor = [0.0] * 2
data1 = ot.Sample(size, 2)  # polygon y = 2x for x in [-25]
data2 = ot.Sample(size, 2)  # polygon y = x*x for x in [-11]
for i in range(size):
    tmp = 7.0 * i / size + 2
    cursor[0] = tmp
    cursor[1] = 2 * tmp
    data1[i] = cursor
    tmp = 9.0 * i / size + 1
    cursor[0] = tmp
    cursor[1] = tmp * tmp
    data2[i] = cursor
graph = ot.Graph("Some polygons", "x1", "x2", True, "topright", 1.0)
myPolygon1 = ot.Polygon(data1)
myPolygon1.setColor("blue")
myPolygon1.setLegend("polygon 1")
graph.add(myPolygon1)
myPolygon2 = ot.Polygon(data2)
myPolygon2.setColor("red")
myPolygon2.setLegend("polygon 2")
graph.add(myPolygon2)
# graph.draw('curve11.png')
view = View(graph)
# view.save('curve11.png')
view.ShowAll(block=True)

# PolygonArray
generator = ot.Normal(2)
size = 5
array = ot.PolygonCollection(size)
palette = ot.Drawable.BuildDefaultPalette(size)
palette[0] = "blue"
for i in range(size):
    vertices = generator.getSample(3)
    array[i] = ot.Polygon(vertices, palette[i], palette[size - i - 1])
graph = ot.Graph("An array of polygons", "x", "y", True, "topright")
parray = ot.PolygonArray(array)
parray.setLegend("array of polys")
graph.add(parray)
# graph.draw('curve12.png')
view = View(graph)
# view.save('curve12.png')

# BipartiteGraph.draw
graph = ot.BipartiteGraph([[0, 1, 2], [0, 1, 2]]).draw()
view = View(graph)

# BarPlot: FORM reliability index marginal parameter sensitivity
f = ot.SymbolicFunction(["E", "F", "L", "I"], ["-F*L^3/(3*E*I)"])
dim = f.getInputDimension()
mean = [50.0, 1.0, 10.0, 5.0]
sigma = ot.Point(dim, 1.0)
R = ot.IdentityMatrix(dim)
distribution = ot.Normal(mean, sigma, R)
vect = ot.RandomVector(distribution)
output = ot.CompositeRandomVector(f, vect)
event = ot.ThresholdEvent(output, ot.Less(), -3.0)
solver = ot.Cobyla()
solver.setMaximumEvaluationNumber(400)
solver.setMaximumAbsoluteError(1.0e-10)
solver.setMaximumRelativeError(1.0e-10)
solver.setMaximumResidualError(1.0e-10)
solver.setMaximumConstraintError(1.0e-10)
algo = ot.FORM(solver, event, mean)
algo.run()
result = algo.getResult()
graph, _ = result.drawHasoferReliabilityIndexSensitivity()
view = View(graph)

# Optimization error history
opt_result = result.getOptimizationResult()
graph = opt_result.drawErrorHistory()
view = View(graph)
view.ShowAll(block=True)

# GridLayout
grid = ot.GridLayout(2, 3)
palette = ot.Drawable.BuildDefaultPalette(10)
for j in range(grid.getNbColumns()):
    alpha = 1.0 + j
    pdf_curve = ot.WeibullMin(1.0, alpha, 0.0).drawPDF()
    cdf_curve = ot.WeibullMin(1.0, alpha, 0.0).drawCDF()
    pdf_curve.setColors([palette[j]])
    cdf_curve.setColors([palette[j]])
    pdf_curve.setLegends(["alpha={}".format(alpha)])
    cdf_curve.setLegends(["alpha={}".format(alpha)])
    grid.setGraph(0, j, pdf_curve)
    grid.setGraph(1, j, cdf_curve)
view = View(grid)

# Square axes
graph = ot.ClaytonCopula(5.0).drawPDF()
view = View(graph, square_axes=True)

# Show axes as prescribed by getAxes()
graph = ot.Normal().drawPDF()
graph.setAxes(False)
view = View(graph)

# test _repr_png_
png = graph._repr_png_()
assert b"PNG" in png[:10]

# BuildDefaultPalette, BuildTableauPalette
ncurves = 5
graph = ot.Graph("BuildPalette", "X", "Y", True, "topright")
n = 20
x = ot.Sample([[i] for i in range(n)])
for i in range(ncurves):
    y = ot.Normal().getSample(n)
    curve = ot.Curve(x, y)
    curve.setLegend("Curve #%d" % (i))
    graph.add(curve)
palette = ot.Drawable.BuildDefaultPalette(ncurves)
graph.setColors(palette)
view = View(graph)
palette = ot.Drawable.BuildTableauPalette(ncurves)
graph.setColors(palette)
view = View(graph)

# mixed legend
f = ot.SymbolicFunction(["x", "y"], ["sin(x)*sin(y)"])
graph = f.draw([-4.0] * 2, [4.0] * 2, [64] * 2)
curve = ot.Curve([-4.0, 4.0], [1.0, 1.0], "curve")
curve.setColor("black")
curve.setLineStyle("dashed")
graph.add(curve)
cloud = ot.Cloud(ot.Normal(2).getSample(100), "cloud")
graph.add(cloud)
view = View(graph)

view.ShowAll(block=True)

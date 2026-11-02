import openturns as ot
import openturns.experimental as otexp
import openturns.viewer as otv

ot.RandomGenerator.SetSeed(0)

# The reference measure is the uniform law on the square, and the
# approximation space is spanned by the first ten functions of the tensor
# Legendre basis, enumerated in linear (total-degree) order.
factory = ot.OrthogonalProductPolynomialFactory([ot.Uniform(-1.0, 1.0)] * 2)
basis = ot.OrthogonalBasis(factory)
distribution = otexp.ChristoffelDistribution(basis, 10)

xMin = [-1.0] * 2
xMax = [1.0] * 2
ot.ResourceMap.SetAsBool("Contour-DefaultIsFilled", True)

# The Christoffel density concentrates at the four corners of the square,
# where the Legendre polynomials take their largest values.
pdfGraph = distribution.drawPDF(xMin, xMax, [81] * 2)
pdfGraph.setTitle("PDF")
pdfGraph.setXTitle("$x_1$")
pdfGraph.setYTitle("$x_2$")

# The CDF ranges from 0 to 1, so its contour levels are set explicitly: the
# default levels are empirical quantiles of the grid values and would stop
# below 1.
cdfGraph = distribution.drawCDF(xMin, xMax, [41] * 2)
cdfDrawable = cdfGraph.getDrawable(0)
cdfDrawable.setLevels(ot.Point([0.1 * i for i in range(11)]))
cdfGraph.setDrawable(0, cdfDrawable)
cdfGraph.setTitle("CDF")
cdfGraph.setXTitle("$x_1$")
cdfGraph.setYTitle("$x_2$")

grid = ot.GridLayout(1, 2)
grid.setGraph(0, 0, pdfGraph)
grid.setGraph(0, 1, cdfGraph)
grid.setTitle("ChristoffelDistribution")
otv.View(grid, figure_kw={"figsize": (10.0, 4.5)}, square_axes=True)

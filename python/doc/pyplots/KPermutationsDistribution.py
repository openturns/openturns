import openturns as ot
from openturns.viewer import View

grid = ot.GridLayout(1, 2)
pdf_2d = ot.Graph("KPermutations, PDF", "x1", "x2", True)
cdf_2d = ot.Graph("KPermutations, CDF", "x1", "x2", True)

distribution_2d = ot.KPermutationsDistribution(2, 10)

xMin = [-0.1] * 2
xMax = [10.1] * 2

pdf_2d.add(distribution_2d.drawPDF(xMin, xMax))
pdf_2d.setLegends([""])
cdf_2d.add(distribution_2d.drawCDF(xMin, xMax, [101] * 2))
cdf_2d.setLegends([""])
grid.setGraph(0, 0, pdf_2d)
grid.setGraph(0, 1, cdf_2d)
grid.setTitle("KPermutationsDistribution")
grid.setLegendPosition("upper right")
v = View(grid)
fig = v.getFigure()
fig.axes[1].legend(loc="best")

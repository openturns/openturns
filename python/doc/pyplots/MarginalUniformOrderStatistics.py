import openturns as ot
import openturns.experimental as otexp
from openturns.viewer import View

grid = ot.GridLayout(1, 2)
pdf_2d = ot.Graph("Marginal uniform order statistics, PDF", "x1", "x2", True)
cdf_2d = ot.Graph("Marginal uniform order statistics, CDF", "x1", "x2", True)

distribution_2d = otexp.MarginalUniformOrderStatistics(4, [1, 3])

cloud = ot.Cloud(distribution_2d.getSample(1000))

xMin = [-0.1] * 2
xMax = [1.1] * 2

pdf_2d.add(distribution_2d.drawPDF(xMin, xMax))
pdf_2d.setLegends([""])
pdf_2d.add(cloud)
cdf_2d.add(distribution_2d.drawCDF(xMin, xMax, [71] * 2))
cdf_2d.setLegends([""])
grid.setGraph(0, 0, pdf_2d)
grid.setGraph(0, 1, cdf_2d)
grid.setTitle("MarginalUniformOrderStatistics")
grid.setLegendPosition("upper right")
v = View(grid)
fig = v.getFigure()
fig.axes[1].legend(loc="best")

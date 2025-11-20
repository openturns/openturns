import openturns as ot
import openturns.viewer as otv

grid = ot.GridLayout(1, 2)
pdf_2d = ot.Graph("Uniform order statistics, PDF", "x1", "x2", True)
cdf_2d = ot.Graph("Uniform order statistics, CDF", "x1", "x2", True)

distribution_2d = ot.UniformOrderStatistics(2)

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
grid.setTitle("UniformOrderStatistics")
grid.setLegendPosition("upper right")
v = otv.View(grid)
fig = v.getFigure()
fig.axes[1].legend(loc="best")

import openturns as ot
import openturns.experimental as otexp
import openturns.viewer as otv

grid = ot.GridLayout(1, 2)
pdf_2d = ot.Graph("MultivariatePolya, PDF", "x1", "x2")
cdf_2d = ot.Graph("MultivariatePolya, CDF", "x1", "x2")

distribution_2d = otexp.MultivariatePolya(10, [2.0, 3.0], 1.0)

xMin = [-0.1] * 2
xMax = [10.1] * 2

pdf_2d.add(distribution_2d.drawPDF(xMin, xMax))
pdf_2d.setLegends([""])
cdf_2d.add(distribution_2d.drawCDF(xMin, xMax, [101] * 2))
cdf_2d.setLegends([""])
grid.setGraph(0, 0, pdf_2d)
grid.setGraph(0, 1, cdf_2d)
grid.setTitle("MultivariatePolya")
grid.setLegendPosition("upper right")
v = otv.View(grid)
fig = v.getFigure()
fig.axes[1].legend(loc="best")

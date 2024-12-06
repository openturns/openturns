import openturns as ot
import openturns.experimental as otexp
from openturns.viewer import View

grid = ot.GridLayout(1, 2)
pdf_2d = ot.Graph("CombinationsDistribution, PDF", "x1", "x2", True)
cdf_2d = ot.Graph("CombinationsDistribution, CDF", "x1", "x2", True)

distribution_2d = otexp.CombinationsDistribution(2, 10)

xMin = [-0.1] * 2
xMax = [10.1] * 2

pdf_2d.add(distribution_2d.drawPDF(xMin, xMax))
pdf_2d.setLegends([""])
cdf_2d.add(distribution_2d.drawCDF(xMin, xMax, [101] * 2))
cdf_2d.setLegends([""])
grid.setGraph(0, 0, pdf_2d)
grid.setGraph(0, 1, cdf_2d)
grid.setTitle("CombinationsDistribution")
View(grid).ShowAll()

import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View

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
graph = grid

fig = plt.figure(figsize=(12, 8))
View(graph, figure=fig, add_legend=True)

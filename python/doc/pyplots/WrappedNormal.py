import openturns as ot
import openturns.experimental as otexp
from matplotlib import pyplot as plt
import openturns.viewer as otv

ot.RandomGenerator.SetSeed(0)

pdf_graph = ot.Graph("PDF graph", "x", "PDF")
pdf_graph.setLegendPosition("upper left")
palette = ot.Drawable.BuildDefaultPalette(4)
for i, sigma in enumerate([0.5, 1.0, 2.0, 3.0]):
    distribution = otexp.WrappedNormal([0.0], ot.CovarianceMatrix([[sigma**2]]))
    pdf_curve = distribution.drawPDF(-1.0, 1.0, 256).getDrawable(0)
    pdf_curve.setColor(palette[i])
    pdf_curve.setLegend(f"sigma={sigma}")
    pdf_graph.add(pdf_curve)
fig = plt.figure(figsize=(10, 4))
pdf_axis = fig.add_subplot(121)
otv.View(pdf_graph, figure=fig, axes=[pdf_axis], add_legend=True)
fig.suptitle("WrappedNormal(0,sigma^2) on the circle")
plt.subplots_adjust(wspace=0.3)

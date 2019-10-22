import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View

pdf_graph = ot.Graph('PDF graph', 'x', 'PDF', True, 'topleft')
cdf_graph = ot.Graph('CDF graph', 'x', 'CDF', True, 'topleft')
palette = ot.Drawable.BuildDefaultPalette(10)
for i, p in enumerate([1.0,  2.0, 3.0]):
    distribution = ot.WeibullMax(1.0, p, 0.0)
    pdf_curve = distribution.drawPDF().getDrawable(0)
    cdf_curve = distribution.drawCDF().getDrawable(0)
    pdf_curve.setColor(palette[i])
    cdf_curve.setColor(palette[i])
    pdf_curve.setLegend('alpha={}'.format(p))
    cdf_curve.setLegend('alpha={}'.format(p))
    pdf_graph.add(pdf_curve)
    cdf_graph.add(cdf_curve)
fig = plt.figure(figsize=(10, 4))
pdf_axis = fig.add_subplot(121)
cdf_axis = fig.add_subplot(122)
View(pdf_graph, figure=fig, axes=[pdf_axis], add_legend=True)
View(cdf_graph, figure=fig, axes=[cdf_axis], add_legend=True)
fig.suptitle('WeibullMax(1,alpha,0)')

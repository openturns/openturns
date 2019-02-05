import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View

pdf_graph = ot.Graph('PDF graph', 'x', 'PDF', True, 'topleft')
cdf_graph = ot.Graph('CDF graph', 'x', 'CDF', True, 'topleft')
palette = ot.Drawable.BuildDefaultPalette(10)
for i, p in enumerate([(0.5,1),(5,6),(1,4),(2,4),(2,7)]):
    r, t = p
    distribution = ot.Beta(r, t, -1.0, 1.0)
    pdf_curve = distribution.drawPDF().getDrawable(0)
    cdf_curve = distribution.drawCDF().getDrawable(0)
    pdf_curve.setColor(palette[i])
    cdf_curve.setColor(palette[i])
    pdf_curve.setLegend('r,t={},{}'.format(r, t))
    cdf_curve.setLegend('r,t={},{}'.format(r, t))
    pdf_graph.add(pdf_curve)
    cdf_graph.add(cdf_curve)
fig = plt.figure(figsize=(10, 4))
pdf_axis = fig.add_subplot(121)
cdf_axis = fig.add_subplot(122)
View(pdf_graph, figure=fig, axes=[pdf_axis], add_legend=True)
View(cdf_graph, figure=fig, axes=[cdf_axis], add_legend=True)
fig.suptitle('Beta(r,t,-1,1)')

import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View

distribution = ot.NormalGamma()

distribution.setDescription(["$X$", "$Y$"])
pdf_graph = distribution.drawPDF()
pdf_contour = pdf_graph.getDrawable(0).getImplementation()
pdf_contour.setColorBarPosition("")
pdf_contour.setColorMapNorm("rank")
pdf_graph.setDrawable(0, pdf_contour)
pdf_graph.setTitle(str(distribution))
fig = plt.figure(figsize=(5, 5))
pdf_axis = fig.add_subplot(111)
View(pdf_graph, figure=fig, axes=[pdf_axis], add_legend=False)

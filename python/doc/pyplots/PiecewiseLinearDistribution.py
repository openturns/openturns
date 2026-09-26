import openturns as ot
import openturns.experimental as otexp
import openturns.viewer as otv
from matplotlib import pyplot as plt
import math

ot.RandomGenerator.SetSeed(0)

n_points = 21
x = ot.RegularGrid(-2.0 * math.pi, 4.0 * math.pi / (n_points - 1), n_points).getVertices().asPoint()
y = [math.sin(xi) + 1.5 for xi in x]
distribution = otexp.PiecewiseLinearDistribution(x, y)

fig = plt.figure(figsize=(10, 4))

# PDF
ax_pdf = fig.add_subplot(121)
pdf_graph = distribution.drawPDF()
otv.View(pdf_graph, figure=fig, axes=[ax_pdf], add_legend=False)
ax_pdf.set_title("PDF")

# CDF
ax_cdf = fig.add_subplot(122)
cdf_graph = distribution.drawCDF()
otv.View(cdf_graph, figure=fig, axes=[ax_cdf], add_legend=False)
ax_cdf.set_title("CDF")

fig.suptitle("PiecewiseLinearDistribution discretizing $x \\mapsto \\sin(x) + 1.5$ on $[-2\\pi, 2\\pi]$")

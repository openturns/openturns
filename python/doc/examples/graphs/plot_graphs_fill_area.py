"""
How to fill an area
===================
"""
# sphinx_gallery_thumbnail_number = 2
# %%

# %%
# In this example, we show how to fill specified area with a given color.

# %%
import openturns as ot
from openturns import viewer
from matplotlib import pylab as plt

ot.Log.Show(ot.Log.NONE)

# %%
# We generate a sample from a standard gaussian distribution.

# %%
dist = ot.Normal()
graph = dist.drawPDF()
view = viewer.View(graph)

# %%


def linearSample(xmin, xmax, npoints):
    """Returns a sample created from a regular grid
    from xmin to xmax with npoints points."""
    step = (xmax - xmin) / (npoints - 1)
    rg = ot.RegularGrid(xmin, step, npoints)
    vertices = rg.getVertices()
    return vertices


# %%
a = 1.0
b = 2.0

# %%
nplot = 100  # Number of points in the plot
x = linearSample(a, b, nplot)
y = dist.computePDF(x)


# %%
# Compute the bounds to fill: the lower vertical bound is 0 and the upper vertical bound is the PDF.


# %%
vLow = [0.0] * nplot
vUp = [y[i, 0] for i in range(nplot)]

# %%
area = dist.computeCDF(b) - dist.computeCDF(a)

# %%
boundsPoly = ot.Polygon.FillBetween(x.asPoint(), vLow, vUp)
graph = dist.drawPDF()
graph.add(boundsPoly)
graph.setTitle("Area = %.3f" % (area))
graph.setLegends([""])
view = viewer.View(graph)
plt.show()

# %%
# The CDF difference is equal to the area under the curve.

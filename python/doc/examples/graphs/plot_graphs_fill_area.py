"""
How to fill an area
===================
"""
# sphinx_gallery_thumbnail_number = 2
# %%

# %%
# In this example, we show how to fill a specified 2D area with a given color.
# The bounds of the area are defined by the X coordinates and the
# Y lower and upper bounds.

# %%
import openturns as ot
import openturns.viewer as viewer
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
nplot = 100  # Number of points in the plot
x = linearSample(a, b, nplot)
y = dist.computePDF(x)


# %%
# The following function uses the `PolygonArray` class to create a area filled with a given color.

# %%
def fillWithColorInBetween(x, yLower, yUpper, color):
    """
    Fill an area with a color between two bounds

    Parameters
    ----------
    x : ot.Point(n_points)
        The X coordinates.
    yLower : ot.Point(n_points)
        The Y coordinate of the lower bound.
    yUpper : ot.Point(n_points)
        The Y coordinate of the upper bound.

    Return
    ------
    polygonArray : ot.PolygonArray()
        The colored area.
    """
    n_points = x.getDimension()
    if yLower.getDimension() != n_points:
        raise ValueError(
            f"The number of Y lower bounds is equal to {yLower.getDimension()} "
            f"but the number of X coordinates is {n_points}"
        )
    if yUpper.getDimension() != n_points:
        raise ValueError(
            f"The number of Y upper bounds is equal to {yUpper.getDimension()} "
            f"but the number of X coordinates is {n_points}"
        )
    n_points = x.getDimension()
    polyData = []
    # Add the lower bound, forward
    for i in range(n_points):
        polyData.append([x[i], yLower[i]])
    # Add the upper bound, backward
    for i in range(n_points - 1, -1, -1):
        polyData.append([x[i], yUpper[i]])

    polyData = ot.Sample(polyData)
    polygonList = [
        ot.Polygon(polyData, color, color)
    ]
    polygonArray = ot.PolygonArray(polygonList)
    return polygonArray


# %%
# Compute the bounds to fill: the lower vertical bound is zero and the upper vertical bound is the PDF.
xPoint = x.asPoint()
yLower = ot.Point(nplot)
yUpper = y.asPoint()

# %%
area = dist.computeCDF(b) - dist.computeCDF(a)

# %%
palette = ot.Drawable.BuildDefaultPalette(2)
graph = dist.drawPDF()
color = palette[0]
boundsPoly = fillWithColorInBetween(xPoint, yLower, yUpper, color)
graph.add(boundsPoly)
graph.setTitle("Area = %.3f" % (area))
graph.setLegends([""])
view = viewer.View(graph)
plt.show()

# %%
# The CDF difference is equal to the area under the curve.

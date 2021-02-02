"""
A quick start guide to graphs 
==============================
"""
# sphinx_gallery_thumbnail_number = 4
# %% 
#
# In this example, we show how to create graphs. We show how to create and configure its axes and its colors. We show how to create a plot based on the combination of several plots.

# %%
# The `draw` method the `Graph` class
# -----------------------------------
#
# The simplest way to create a graphics is to use the `draw` method. The `Normal` distribution for example provides a method to draw the density function of the gaussian distribution. 

# %%
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
ot.Log.Show(ot.Log.NONE)

# %%
n = ot.Normal()
n

# %%
graph = n.drawPDF()
view = viewer.View(graph)

# %%
# To configure the look of the plot, we can first observe the type of graphics returned by the `drawPDF` method returns: it is a `Graph`.

# %%
graph = n.drawPDF()
type(graph)

# %%
# The `Graph` class provides several methods to configure the legends, the title and the colors. Since a graphics  can contain several sub-graphics, the `setColors` takes a list of colors as inputs argument: each item of the list corresponds to the sub-graphics.

# %%
graph.setXTitle("N")
graph.setYTitle("PDF")
graph.setTitle("Probability density function of the standard gaussian distribution")
graph.setLegends(["N"])
graph.setColors(["blue"])
view = viewer.View(graph)

# %%
# Combine several graphics
# ------------------------
#
# In order to combine several graphics, we can use the `add` method.

# %%
# Let us create an empirical histogram from a sample.

# %%
sample = n.getSample(100)

# %%
histo = ot.HistogramFactory().build(sample).drawPDF()
view = viewer.View(histo)

# %%
# Then we add the histogram to the `graph` with the `add` method. The `graph` then contains two plots.

# %%
graph.add(histo)
view = viewer.View(graph)

# %%
# Draw a cloud
# ------------
#
# The `Cloud` class creates clouds of bidimensional points. To demonstrate it, let us create two gaussian distributions in two dimensions.

# %%
# Create a Funky distribution
corr = ot.CorrelationMatrix(2)
corr[0, 1] = 0.2
copula = ot.NormalCopula(corr)
x1 = ot.Normal(-1., 1)
x2 = ot.Normal(2, 1)
x_funk = ot.ComposedDistribution([x1, x2], copula)

# %%
# Create a Punk distribution
x1 = ot.Normal(1.,1)
x2 = ot.Normal(-2,1)
x_punk = ot.ComposedDistribution([x1, x2], copula)

# %%
# Let us mix these two distributions.

# %%
mixture = ot.Mixture([x_funk, x_punk], [0.5,1.])

# %%
n=500
sample = mixture.getSample(n)

# %%
graph = ot.Graph("n=%d" % (n), "X1", "X2", True, '')
cloud = ot.Cloud(sample)
graph.add(cloud)
view = viewer.View(graph)

# %%
# We sometimes want to customize the graphics by choosing the type of point (square, triangle, circle, etc...), of line (continuous, dashed, etc...) or another parameter. We can know the list of possible values with the corresponding `getValid` method. 
#
# For example, the following function returns the possible values of the `PointStyle` parameter.

# %%
ot.Drawable.GetValidPointStyles()

# %%
# The following method returns the list of colors.

# %%
ot.Drawable.GetValidColors()[0:10]

# %%
# In the following graphics, we use the "aquamarine1" color with "fcircle" circles.

# %%
graph = ot.Graph("n=%d" % (n), "X1", "X2", True, '')
cloud = ot.Cloud(sample)
cloud.setColor("aquamarine1")
cloud.setPointStyle("fcircle")
graph.add(cloud)
view = viewer.View(graph)

# %%
# Configure the style of points and the thickness of a curve
# ----------------------------------------------------------
#
# Assume that we want to plot the sine curve from -2 to 2. The simplest way is to use the `draw` method of the function.

# %%
g = ot.SymbolicFunction("x","sin(x)")

# %%
graph = g.draw(-2,2)
view = viewer.View(graph)

# %%
# I would rather get a dashed curve: let us search for the available line styles. 

# %%
ot.Drawable.GetValidLineStyles()


# %%
# In order to use the `Curve` class, it will be easier if we have a method to generate a `Sample` containing points regularly spaced in an interval.

# %%
def linearSample(xmin,xmax,npoints):
    '''Returns a sample created from a regular grid 
    from xmin to xmax with npoints points.'''
    step = (xmax-xmin)/(npoints-1)
    rg = ot.RegularGrid(xmin, step, npoints)
    vertices = rg.getVertices()
    return vertices


# %%
x = linearSample(-2,2,50)
y = g(x)

# %%
graph = ot.Graph("Sinus","x","sin(x)",True)
curve = ot.Curve(x,y)
curve.setLineStyle("dashed")
curve.setLineWidth(4)
graph.add(curve)
view = viewer.View(graph)


# %%
# Create colored curves
# ---------------------
#
# In some situations, we want to create curves with different colors. In this case, the following function generates a color corresponding to the `indexCurve` integer in a ensemble of `maximumNumberOfCurves` curves.

# %%
def createHSVColor(indexCurve,maximumNumberOfCurves):
    '''Create a HSV color for the indexCurve-th curve 
    from a sample with maximum size equal to maximumNumberOfCurves'''
    color = ot.Drawable.ConvertFromHSV(indexCurve * 360.0/maximumNumberOfCurves, 1.0, 1.0)
    return color


# %%
pofa = ot.HermiteFactory()

# %%
graph = ot.Graph("Orthonormal Hermite polynomials","x","y",True,"bottomright")
degreemax = 5
for k in range(degreemax):
    pk = pofa.build(k)
    curve = pk.draw(-3.,3.,50)
    curve.setLegends(["P%d" % (k)])
    curve.setColors([createHSVColor(k,degreemax)])
    graph.add(curve)
view = viewer.View(graph)

# %%
# Create matrices of graphics
# ---------------------------
#
# The library does *not* has objects to create a grid of graphics. However, we can use the `add_subplot` function from Matplotlib.
#
# Let us create two graphics of the PDF and CDF of the following gaussian distribution..

# %%
n = ot.Normal()
myPDF = n.drawPDF()
myCDF = n.drawCDF()

# %%
import pylab as pl
import openturns.viewer as otv

# %%
# We create a figure with the `figure` function from Matplotlib, then we add two graphics with the `add_subplot` function. We use the `viewer.View` function to create the required Matplotlib object. Since we are not interested by the output of the `View` function, we use the dummy variable `_` as output. The title is finally configured with `suptitle`.

# %%
fig = pl.figure(figsize=(12, 4))
ax_pdf = fig.add_subplot(1, 2, 1)
_ = otv.View(myPDF, figure=fig, axes=[ax_pdf])
ax_cdf = fig.add_subplot(1, 2, 2)
_ = otv.View(myCDF, figure=fig, axes=[ax_cdf])
_ = fig.suptitle("The gaussian")

# %%
# Save a plot into a file
# -----------------------

# %%
# The `View` class has a `save` method which saves the graph into an image.

# %%
import openturns.viewer as otv

# %%
n = ot.Normal()
graph = n.drawPDF()
view = otv.View(graph)
view.save("normal.png")

# %%
# We can use the `dpi` option to configure the resolution in dots per inch.

# %%
view.save("normal-100dpi.png", dpi=100)

# %%
# Configure the size of a graph with matplotlib
# ---------------------------------------------

# %%
import openturns.viewer as otv

# %%
# We first create a graph containing the PDF of a gaussian distribution

# %%
n = ot.Normal()
graph = n.drawPDF()

# %%
# The `figure_kw` keyword argument sets the optional arguments of the figure. In the following statement, we set the figure size in inches

# %%
view = otv.View(graph, figure_kw = {"figsize": (12, 8)})

# %%
# The `getFigure` method returns the current figure. This allows to configure it as any other Matplotlib figure. In the following example, we configure the `suptitle`.

# %%
fig = view.getFigure()
fig.suptitle("The suptitle")
fig

# %%
# The `plot_kw` optional argument sets the arguments of the plot. In the following example, we set the color of the plot in blue.

# %%
view = otv.View(graph, plot_kw={'color':'blue'})
plt.show()

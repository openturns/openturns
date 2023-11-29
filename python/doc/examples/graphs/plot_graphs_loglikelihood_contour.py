"""
Plot the log-likelihood contours of a distribution
==================================================
"""
# sphinx_gallery_thumbnail_number = 5
# %%

# %%
# In this example, we show how to plot the bidimensionnal log-likelihood contours of function given a sample.

# %%
from matplotlib import pylab as plt
import openturns.viewer as viewer
import openturns as ot

ot.RandomGenerator.SetSeed(0)
ot.Log.Show(ot.Log.NONE)

# %%
# Generate a sample
# -----------------

# %%
# We create a `TruncatedNormal` and generate a small sample.

# %%
a = -1
b = 2.5
mu = 2.0
sigma = 3.0
distribution = ot.TruncatedNormal(mu, sigma, a, b)
sample = distribution.getSample(11)

# %%
# In order to see the distribution and the sample, we draw the PDF of the distribution and generate a clouds which X coordinates are the sample values.

# %%
graph = distribution.drawPDF()
graph.setLegends(["TruncatedNormal"])
graph.setColors(["red"])
zeros = ot.Sample(sample.getSize(), 1)
cloud = ot.Cloud(sample, zeros)
cloud.setLegend("Sample")
graph.add(cloud)
graph.setLegendPosition("upper left")
view = viewer.View(graph)


# %%
# The following function computes the log-likelihood of a `TruncatedNormal`
# which mean and standard deviations are given as input arguments.
# The lower and upper bounds of the distribution are computed as minimum and maximum of the sample.

# %%
# Define the log-likelihood function
# ----------------------------------

# %%
# The following function evaluates the log-likelihood function given a point :math:`X=(\mu,\sigma`).
# In order to evaluate the likelihood on the sample, we use a trick: we evaluate
# the `computeMean` method on the log-PDF sample, then multiply by the sample size.
# This is much faster than using a `for` loop.


# %%
def logLikelihood(X):
    """
    Evaluate the log-likelihood of a TruncatedNormal on a sample.
    """
    samplesize = sample.getSize()
    mu = X[0]
    sigma = X[1]
    a = sample.getMin()[0]
    b = sample.getMax()[0]
    delta = (b - a) / samplesize
    a -= delta
    b += delta
    distribution = ot.TruncatedNormal(mu, sigma, a, b)
    samplelogpdf = distribution.computeLogPDF(sample)
    loglikelihood = samplelogpdf.computeMean() * samplesize
    return loglikelihood


# %%
# With the draw method
# --------------------

# %%
# In this section, we use the `draw` method which is available for any `Function` which has 1 or 2 input arguments.
# In our case, the log-likelihood function has two inputs: :math:`x_0=\mu` and :math:`x_1=\sigma`.

# %%
# Draw the log-likelihood function with the `draw` method: this is much faster than using a `for` loop.
# In order to print LaTeX X and Y labels, we use the `"r"` character in front of the string containing the LaTeX command.

# %%
logLikelihoodFunction = ot.PythonFunction(2, 1, logLikelihood)
graphBasic = logLikelihoodFunction.draw([-3.0, 0.1], [5.0, 7.0], [50] * 2)
graphBasic.setXTitle(r"$\mu$")
graphBasic.setYTitle(r"$\sigma$")
view = viewer.View(graphBasic)

# %%
# Customizing the number of levels
# --------------------------------

# %%
# The level values are computed from the quantiles of the data, so that the contours are equally spaced.
# We can configure the number of levels by setting the `Contour-DefaultLevelsNumber` key in the `ResourceMap`.

# %%
ot.ResourceMap.SetAsUnsignedInteger("Contour-DefaultLevelsNumber", 6)
logLikelihoodFunction = ot.PythonFunction(2, 1, logLikelihood)
graphBasic = logLikelihoodFunction.draw([-3.0, 0.1], [5.0, 7.0], [50] * 2)
graphBasic.setXTitle(r"$\mu$")
graphBasic.setYTitle(r"$\sigma$")
view = viewer.View(graphBasic)

# %%
# A part of the graphics is hidden by the legends. This is why we fine tune the graphics in the next examples.

# %%
# Getting the level values
# ------------------------

# %%
# The level values can be retrieved with the `getLevels` method.

# %%
drawables = graphBasic.getDrawables()
levels = []
for contours in drawables:
    levels.append(contours.getLevels()[0])
levels

# %%
# Monochrome contour plot
# -----------------------

# %%
# We first configure the contour plot.
# By default each level is a dedicated contour in order to have one color per contour,
# but they all share the same grid and data.
# We use the `getDrawable` method to take the first contour as the only one with multiple levels.
# Then we use the `setLevels` method: we ask for many iso-values in the same data so the color will be the same for all curves.
# In order to inline the level values labels, we use the `setDrawLabels` method.

# %%
contours = graphBasic.getDrawable(0)
contours.setLevels(levels)
contours.setDrawLabels(True)
contours.setColor("red")

# %%
# Then we create a new graph. Finally, we use the `setDrawables` to substitute the collection of drawables by a collection reduced to this unique contour.

# %%
graphFineTune = ot.Graph("Log-Likelihood", r"$\mu$", r"$\sigma$", True, "")
graphFineTune.setDrawables([contours])
graphFineTune.setLegendPosition("")  # Remove the legend
view = viewer.View(graphFineTune)

# %%
# Grayscale contour plot
# ----------------------

# %%
# The following script applies a grayscale palette to the contour plot.
# We adjust the white and black margins using the `Drawable-WhiteMargin` and `Drawable-BlackMargin` keys in the `ResourceMap`
# in order to use colors between `#cdcdcd` and `#141414`.
# Then we create the `drawables` list, where each item is a single contour with its own level and color.

# %%
# Take the first contour as the only one with multiple levels
contour = graphBasic.getDrawable(0)
# Build a range of colors
ot.ResourceMap.SetAsUnsignedInteger("Drawable-BlackMargin", 20)
ot.ResourceMap.SetAsUnsignedInteger("Drawable-WhiteMargin", 50)
palette = ot.Drawable.BuildGrayScalePalette(len(levels))
# Create the drawables list, appending each contour with its own color
drawables = []
for level in levels:
    contour.setLevels([level])
    # Inline the level values
    contour.setDrawLabels(True)
    # We have to copy the drawable because a Python list stores only pointers
    drawables.append(ot.Drawable(contour))

# %%
graphFineTune = ot.Graph("Log-Likelihood", r"$\mu$", r"$\sigma$", True, "")
graphFineTune.setDrawables(drawables)  # Replace the drawables
graphFineTune.setLegendPosition("")  # Remove the legend
graphFineTune.setColors(palette)  # Add colors
view = viewer.View(graphFineTune)
plt.show()

# %%
# Reset default settings
ot.ResourceMap.Reload()

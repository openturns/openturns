"""
A quick start guide to contours
===============================
"""
# sphinx_gallery_thumbnail_number = 6
# %%
#
# In this example we show how to create contour graphs and how to make the most of their display settings.

# %%
# The `draw` method, the `Graph` and `Contour` classes
# ----------------------------------------------------
#
# The simplest way to create a contour graph is to use the `draw` method of a bidimensional function.

# %%
import openturns as ot
import openturns.viewer as viewer

# %%
# We build a bidimensional function (function of x and y), define the study domain and the sample size

# %%
f = ot . SymbolicFunction(['x', 'y'], ['exp(-sin(cos(y)^2*x^2+sin(x)^2*y^2))'])
XMin = -5.
XMax = 5.
YMin = -5.
YMax = 5.
NX = 75
NY = 75

# %%
# We build the graph by calling the `draw` method and display it

# %%
graph = f.draw([YMin, YMin], [XMax, YMax], [NX, NY])
viewer.View(graph)

# %%
# The graph contains an unique drawable whose implementation is of class `Contour`

# %%
contour = graph.getDrawable(0).getImplementation()
print(type(contour).__name__)

# %%
# Another way to build the contour is to build the data sample and give it to the constructor of the `Contour` class

# %%
inputData = ot.Box([NX, NY]).generate()
inputData *= [XMax - XMin, YMax - YMin]
inputData += [XMin, YMin]
data = f(inputData)

contour = ot.Contour(ot.Box([NX]).generate() * [XMax - XMin] + [XMin], ot.Box([NY]).generate() * [YMax - YMin] + [YMin], data)

# %%
# By creating an empty graph and adding the contour we can display the whole.

# %%
graph = ot.Graph('Complex iso lines', 'u1', 'u2', True)
graph.add(contour)
viewer.View(graph)

# %%
# The previous graph does not show the associated color bar.
# This point can be modified.
# We will also change the color map and the number of contour lines.

# %%
contour.setColorBarPosition('right')
contour.setColorMap('inferno')
contour.buildDefaultLevels(5)
graph.setDrawables([ot.Drawable(contour)])
viewer.View(graph)

# %%
# For such a function, contour lines are not easy to interpret.
# We will modify the contour to use filled areas and hide the labels.
contour.setIsFilled(True)
contour.setDrawLabels(False)
graph.setDrawables([ot.Drawable(contour)])
viewer.View(graph)

# %%
# Sometimes the colors are not very distinct because some levels are very close while others are very far apart.
# In this case, it is possible to add hatching to the surfaces.
# Here we will also use transparency to soften the colors.

# %%
contour.setAlpha(0.3)
contour.setHatches(['/', '\\', '/\\', '+', '*'])
graph.setDrawables([ot.Drawable(contour)])
viewer.View(graph)

# %%
# When the function takes values very different in magnitude, it may be useful to change the norm which is
# used to distribute the colors and to bound the color interval.
# Here we will also let `matplotlib` calculate the levels by not giving any level to the contour

# %%
contour.setNorm('log')
contour.setLevels([])
contour.setExtend('neither')
contour.setVmin(0.5)
contour.setVmax(2)
graph.setDrawables([ot.Drawable(contour)])
viewer.View(graph)

viewer.View.ShowAll()

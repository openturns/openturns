"""
Draw an histogram
=================
"""

# %%
# In this example we are going to a plot a histogram from a unidimensional sample.

# %%
import openturns as ot
import openturns.viewer as otv

# %%
# Create data to visualize
size = 500
normal = ot.Normal(1)
sample = normal.getSample(size)

# %%
# Draw histogram
graph = ot.HistogramFactory().build(sample).drawPDF()
view = otv.View(graph)

# %%
otv.View.ShowAll()

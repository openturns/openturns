"""
Deterministic design of experiments
===================================
"""
# %%
# In this example we present the available deterministic design of experiments.
#
# Four types of deterministic design of experiments are available:
#
# - `Axial`
# - `Factorial`
# - `Composite`
# - `Box`
#
# Each type of deterministic design is discretized differently according to a number of levels.
#
# Functionally speaking, a design is a `Sample` that lies within the unit cube :math:`(0,1)^d` and can be scaled and moved to cover the desired box.

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
ot.Log.Show(ot.Log.NONE)


# %%
# We will use the following function to plot bi-dimensional samples.

# %%
def drawBidimensionalSample(sample, title):
    n = sample.getSize()
    graph = ot.Graph("%s, size=%d" % (title, n), "X1", "X2", True, '')
    cloud = ot.Cloud(sample)
    graph.add(cloud)
    return graph


# %%
# Axial design
# ------------

# %%
levels = [1.0, 1.5, 3.0]
experiment = ot.Axial(2, levels)
sample = experiment.generate()
graph = drawBidimensionalSample(sample,"Axial")
view = viewer.View(graph)

# %%
# Scale and to get desired location.

# %%
sample *= 2.0
sample += [5.0, 8.0]
graph = drawBidimensionalSample(sample,"Axial")
view = viewer.View(graph)

# %%
# Factorial design
# ----------------
#

# %%
experiment = ot.Factorial(2, levels)
sample = experiment.generate()
sample *= 2.0
sample += [5.0, 8.0]
graph = drawBidimensionalSample(sample,"Factorial")
view = viewer.View(graph)

# %%
# Composite design
# ----------------

# %%
experiment = ot.Composite(2, levels)
sample = experiment.generate()
sample *= 2.0
sample += [5.0, 8.0]
graph = drawBidimensionalSample(sample,"Composite")
view = viewer.View(graph)

# %%
# Grid design
# -----------
#

# %%
levels = [3, 4]
experiment = ot.Box(levels)
sample = experiment.generate()
sample *= 2.0
sample += [5.0, 8.0]
graph = drawBidimensionalSample(sample,"Box")
view = viewer.View(graph)
plt.show()

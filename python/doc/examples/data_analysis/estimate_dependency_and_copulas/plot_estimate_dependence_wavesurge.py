"""
Estimate tail dependence coefficients on the wave-surge data
============================================================
"""
# %%
# In this example we estimate the tail dependence coefficient of a bivariate sample
# applied to the concurrent measurements of two oceanographic variables (wave and surge heights) at a single location off south-west England.
# Readers should refer to [coles2001]_ to get more details.
#
# First, we load the wave-surge dataset.
import openturns as ot
import openturns.viewer as otv
from openturns.usecases import coles

data = coles.Coles().wavesurge
print(data[:5])

graph = ot.Graph(
    "Concurent wave and surge heights", "wave (m)", "surge (m)", True, ""
)
cloud = ot.Cloud(data)
cloud.setColor("red")
graph.add(cloud)
view = otv.View(graph)

# %%
# We plot the graph of the function :math:`u \mapsto \chi(u)` and the graph of the function
# :math:`u \mapsto \bar{\chi}(u)`. We conclude that both variables are asymptotially dependent
# as :math:`\chi > 0` and that they are positively correlated as :math:`\hat{\chi} > 0`.
# We can visually deduce the upper tail dependence coefficient :math:`\chi \simeq 0.26` and
# the upper extremal dependence coefficient :math:`\bar{\chi} \simeq 0.5`.
graph1 = ot.VisualTest.DrawUpperTailDependenceFunction(data)
graph2 = ot.VisualTest.DrawUpperExtremalDependenceFunction(data)
grid = ot.GridLayout(1, 2)
grid.setGraph(0, 0, graph1)
grid.setGraph(0, 1, graph2)
view = otv.View(grid)


# %%
otv.View.ShowAll()

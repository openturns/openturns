"""
Estimate tail dependence coefficients on the wind data
======================================================
"""
# %%
# In this example we estimate the tail dependence coefficient of a bivariate sample
# applied to the corresponding annual maximum wind speeds over the period 1944-1983
# at two locations in the United States: Albany, New-York and Hartford, Connecticut.
# Readers should refer to [coles2001]_ to get more details.
#
# First, we load the wave_surge dataset. The speeds are expressed in knot : one knot is equalt to one nautical mile per hour, which means :math:`1.852\, km.h^{-1}`.
import openturns as ot
import openturns.viewer as otv
from openturns.usecases import coles

data = coles.Coles().wind[:, 1:]
print(data[:5])

graph = ot.Graph(
    "Annual maximum wind speeds at Albany (NY) and Hartford (CT)",
    "spped at Albany (knot)",
    "speed at Hartford (knot)",
    True,
    "",
)
cloud = ot.Cloud(data)
cloud.setColor("red")
graph.add(cloud)
view = otv.View(graph)

# %%
# We plot the graph of the function :math:`u \mapsto \chi(u)` and the graph of the function
# :math:`u \mapsto \bar{\chi}(u)`. We conclude that both variables are asymptotially dependent
# as :math:`\chi > 0` and that they are positively correlated as :math:`\hat{\chi} > 0`.
# We cn visually deduce the upper tail dependence coefficient :math:`\chi \simeq 0.26` and
# the upper extremal dependence coefficient :math:`\bar{\chi} \simeq 0.5`.
# Note that the number of data points is so small that the curves seem chaotic. It is difficult, if not impossible, to deduce the value of :math:`\chi` and :math:`\bar{\chi}` from the curves.
graph1 = ot.VisualTest.DrawUpperTailDependenceFunction(data)
graph2 = ot.VisualTest.DrawUpperExtremalDependenceFunction(data)
grid = ot.GridLayout(1, 2)
grid.setGraph(0, 0, graph1)
grid.setGraph(0, 1, graph2)
view = otv.View(grid)

# %%
otv.View.ShowAll()

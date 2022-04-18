"""
Use the Smolyak quadrature
==========================
"""
# %%
# The goal of this example is to present different properties of Smolyak's
# quadrature. 

# %%
import openturns as ot
import openturns.viewer as otv
from matplotlib import pylab as plt

# %%
# In the first example, we plot the nodes different levels of 
# Smolyak-Legendre quadrature. 
uniform = ot.GaussProductExperiment(ot.Uniform(-1.0, 1.0))
collection = [uniform] * 2

# %%
# In the following loop, the level increases from 1 to 6. 
# For each level, we create the associated Smolyak quadrature and 
# plot the associated nodes. 
number_of_rows=2
number_of_columns=3
bounding_box = ot.Interval([-1.05] * 2, [1.05] * 2)
grid = ot.GridLayout(number_of_rows, number_of_columns)
for i in range(number_of_rows):
    for j in range(number_of_columns):
        level = 1 + j + i * number_of_columns
        print("level = ", level)
        experiment = ot.SmolyakExperiment(collection, level)
        nodes, weights = experiment.generateWithWeights()
        sample_size = weights.getDimension()
        graph = ot.Graph(
            r"Level = %d, n = %d" % (level, sample_size), "$x_1$", "$x_2$", True
        )
        cloud = ot.Cloud(nodes)
        cloud.setPointStyle("circle")
        graph.add(cloud)
        graph.setBoundingBox(bounding_box)
        grid.setGraph(i, j, graph)

unit_width = 3.0
total_width = unit_width * number_of_columns
unit_height = unit_width
total_height = unit_height * number_of_rows
view = otv.View(grid, figure_kw={"figsize": (total_width, total_height)})
_ = plt.suptitle("Smolyak-Legendre")
_ = plt.subplots_adjust(wspace=0.4, hspace=0.4)

# %%
# In the previous plot, the number of nodes is denoted by :math:`n`. 
# We see that the number of nodes in the quadrature slowly increases 
# when the quadrature level increases. 

# %%
# Secondly, we want to plot the number of nodes depending on the dimension 
# and the level. 


level_max = 8  # Maximum level
dimension_max = 8  # Maximum dimension
uniform = ot.GaussProductExperiment(ot.Uniform(-1.0, 1.0))
level_list = list(range(1, 1 + level_max))
graph = ot.Graph("Smolyak quadrature", r"$level$", r"$n$", True, "topleft")
palette = ot.Drawable().BuildDefaultPalette(dimension_max - 1)
for dimension in range(1, dimension_max):
    number_of_nodes = ot.Sample(level_max, 1)
    for level in level_list:
        collection = [uniform] * dimension
        experiment = ot.SmolyakExperiment(collection, level)
        nodes, weights = experiment.generateWithWeights()
        size = nodes.getSize()
        number_of_nodes[level - 1, 0] = size
    cloud = ot.Cloud(ot.Sample.BuildFromPoint(level_list), number_of_nodes)
    cloud.setLegend("$d = %d$" % (dimension))
    cloud.setPointStyle("bullet")
    cloud.setColor(palette[dimension - 1])
    graph.add(cloud)
    curve = ot.Curve(ot.Sample.BuildFromPoint(level_list), number_of_nodes)
    curve.setLegend("")
    curve.setLineStyle("dashed")
    curve.setColor(palette[dimension - 1])
    graph.add(curve)
graph.setLogScale(ot.GraphImplementation.LOGY)
view = otv.View(
    graph,
    figure_kw={"figsize": (4.0, 3.0)},
    legend_kw={"bbox_to_anchor": (1.0, 1.0), "loc": "upper left"},
)

plt.show()

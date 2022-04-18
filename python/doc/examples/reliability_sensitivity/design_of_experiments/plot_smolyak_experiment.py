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
# Loop over level from 1 to 6
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
plt.show()

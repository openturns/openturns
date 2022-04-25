"""
Merge nodes in Smolyak quadrature
=================================
"""
# %%
# The goal of this example is to see the effect of the merge algorithm in 
# Smolyak's quadrature. 

# %%
import numpy as np
import openturns as ot
import openturns.viewer as otv

# %%
# The following examples shows how to get the relative and absolute tolerances. 
epsilon_r = ot.ResourceMap.GetAsScalar("SmolyakExperiment-DefaultPointRelativeEpsilon")
epsilon_a = ot.ResourceMap.GetAsScalar("SmolyakExperiment-DefaultPointAbsoluteEpsilon")
print("Default epsilon_r = ", epsilon_r)
print("Default epsilon_a = ", epsilon_a)

# %%
# The following examples shows how to set the relative and absolute tolerances. 
epsilon_r = 1.e-14
epsilon_a = 1.e-14
ot.ResourceMap.SetAsScalar("SmolyakExperiment-DefaultPointRelativeEpsilon", epsilon_r)
ot.ResourceMap.SetAsScalar("SmolyakExperiment-DefaultPointAbsoluteEpsilon", epsilon_a)

 
# %%
# We are interested to the sensitivity of the number of nodes to the tolerances
# of the merging algorithm. 
# The next function takes the level and tolerances as input arguments, 
# and returns the size of Smolyak's quadrature using Gauss-Legendre nodes. 

def computeNumberOfSmolyakNodes(level, epsilon_a, epsilon_r):
    ot.ResourceMap.SetAsScalar("SmolyakExperiment-DefaultPointRelativeEpsilon", epsilon_r)
    ot.ResourceMap.SetAsScalar("SmolyakExperiment-DefaultPointAbsoluteEpsilon", epsilon_a)
    uniform = ot.GaussProductExperiment(ot.Uniform(0.0, 1.0))
    collection = [uniform] * 2
    experiment = ot.SmolyakExperiment(collection, level)
    nodes, weights = experiment.generateWithWeights()
    size = nodes.getSize()
    return size

# %%
# In our experiments, we use :math:`\epsilon = \epsilon_a = \epsilon_r` i.e. 
# the relative and absolute tolerances are set to be equal. 


# %%
# In general, we want the tolerance to be as small as possible. 
# This is because we want the merge algorithm to detect candidate nodes 
# which are truly sumperimposed. 
# Indeed, our goal is to make the algorithm be insensitive to rounding errors, 
# but we do not want to merge two nodes which are significantly different. 
# We first want to see what is the effect when the tolerance is set to 
# a too small value. 

level = 9
epsilon = 0.0
size = computeNumberOfSmolyakNodes(level, epsilon, epsilon)
print("epsilon = %.2e, level = %d, size = %d" % (epsilon, level, size))
epsilon = 1.e-20
size = computeNumberOfSmolyakNodes(level, epsilon, epsilon)
print("epsilon = %.2e, level = %d, size = %d" % (epsilon, level, size))


# %%
# Hence, 283 nodes are produced with a tolerance set to zero, 
# or to a very small value. 

epsilon = 1.e-8
size = computeNumberOfSmolyakNodes(level, epsilon, epsilon)
print("epsilon = %.2e, level = %d, size = %d" % (epsilon, level, size))

# If the tolerance is set to a larger value, then 2 nodes are detected 
# as duplicate, so that the total number of nodes is equal to 281.

# %%
# We conclude that the tolerance must not be set to a too 
# small value.
#
# In the next `for` loops, for each value of the tolerance from :math:`10^{-1}` 
# down to :math:`10^{-20}`, we increase the `level` and see how this changes 
# the number of nodes. 

graph = ot.Graph("Sensitivity to tolerance", "Level", "Size", True)
point_styles = ["circle", "fdiamond", "fsquare", "ftriangleup", "triangledown"]
number_of_epsilons = 5
epsilon_array = np.logspace(-1, -20, number_of_epsilons)
index = 0
for epsilon in epsilon_array:
    size_list = []
    level_list = list(range(1, 10))
    for level in level_list:
        size = computeNumberOfSmolyakNodes(level, epsilon, epsilon)
        print("epsilon = %.2e, level = %d, size = %d" % (epsilon, level, size))
        size_list.append(size)
    cloud = ot.Cloud(level_list, size_list)
    cloud.setLegend("$\epsilon = %.2e$" % (epsilon))
    cloud.setPointStyle(point_styles[index])
    graph.add(cloud)
    index += 1
palette = ot.DrawableImplementation_BuildDefaultPalette(number_of_epsilons)
graph.setColors(palette)
graph.setLegendPosition("topright")
otv.View(graph, figure_kw={"figsize": (4.0, 3.0)}, 
         legend_kw={"bbox_to_anchor":(1.0, 1.0), "loc":"upper left"})

# %%
# We see that changing the tolerance from :math:`10^{-6}` down to :math:`10^{-20}` 
# does not change much the size of Smolyak's quadrature. 
# Using :math:`\epsilon = 10^{-1}` reduces the number of nodes by a too large 
# amount. 
# This is because a relatively large tolerance considers that many candidate 
# nodes are close to each other, so that these nodes are merged. 
# In this case, the quadrature is not accurate enough. 

#
# In the next `for` loop, for each value of the `level` parameter, 
# we decrease the tolerance from :math:`10^{-1}` 
# down to :math:`10^{-5}` and see how this changes the number of nodes. 
 
graph = ot.Graph("Sensitivity to tolerance", "Epsilon", "Size", True)
point_styles = ["circle", "fdiamond", "fsquare", "ftriangleup", "triangledown"]
number_of_levels = 5
number_of_epsilons = 12
level_list = list(range(1, 10, 2))
index = 0
for level in level_list:
    size_list = []
    epsilon_array = np.logspace(0, -5, number_of_epsilons)
    for epsilon in epsilon_array:
        size = computeNumberOfSmolyakNodes(level, epsilon, epsilon)
        print("level = %d, epsilon = %.2e, size = %d" % (level, epsilon, size))
        size_list.append(size)
    cloud = ot.Cloud(epsilon_array, size_list)
    cloud.setLegend("Level = %d" % (level))
    cloud.setPointStyle(point_styles[index])
    graph.add(cloud)
    index += 1
palette = ot.DrawableImplementation_BuildDefaultPalette(number_of_epsilons)
graph.setColors(palette)
graph.setLegendPosition("topright")
graph.setLogScale(ot.GraphImplementation.LOGXY)
otv.View(graph, figure_kw={"figsize": (4.0, 3.0)}, 
         legend_kw={"bbox_to_anchor":(1.0, 1.0), "loc":"upper left"})

# %%
# We see that changing the tolerance from :math:`1` down to :math:`10^{-3}` 
# changes the number of nodes. 
# Below this threshold, the number of nodes does not change much. 

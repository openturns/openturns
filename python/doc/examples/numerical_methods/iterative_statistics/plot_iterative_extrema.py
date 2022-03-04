"""
Estimate iterative extrema
==========================
"""

# %%
# In this example, we use the :class:`~openturns.IterativeExtrema` class to compute iterative extrema.

# %%
import openturns as ot
import openturns.viewer as otv
ot.RandomGenerator.SetSeed(0)

# %%
# We first create a one-dimensional Gaussian random variable to generate data.
dim = 1
distNormal = ot.Normal(dim)

# %%
# Then we update the extrema of the data iteratively with the :class:`~openturns.IterativeExtrema` class by giving the dimension (here 1):

# %%
iterExtrema = ot.IterativeExtrema(dim)

# %%
# We can now perform the simulations.
# In our case most of the data should be in the [-3,3] interval. Consequently with few samples the expected minimum should be around -3 and the expected maximum should be around 3.
# We first increment the object with one :class:`~openturns.Point` at a time.
# At any given step the current minimum is obtained thanks to the :meth:`~openturns.IterativeExtrema.getMin` method, the current maximum with the :meth:`~openturns.IterativeExtrema.getMax` method and the
# current number of iterations is given by the :meth:`~openturns.IterativeMoments.getIteration` method.

# %%
size = 2000
minEvolution = ot.Sample()
maxEvolution = ot.Sample()
for i in range(size):
    point = distNormal.getRealization()
    iterExtrema.increment(point)
    minEvolution.add(iterExtrema.getMin())
    maxEvolution.add(iterExtrema.getMax())

# %%
# We display the evolution of the minimum (in blue) and the maximum (orange).

# %%
curveMin = ot.Curve(minEvolution, "blue")
curveMin.setColor("blue")
curveMin.setLegend("minimum")

# %%
curveMax = ot.Curve(maxEvolution)
curveMax.setColor("darkorange1")
curveMax.setLegend("maximum")

# %%
graph = ot.Graph("Evolution of the min/max", "iteration nb", "min/max", True)
graph.add(curveMin)
graph.add(curveMax)
graph.setLegends(["minimum", "maximum"])
graph.setLegendPosition("topright")
view = otv.View(graph)

# %%
# We can also increment with a :class:`~openturns.Sample`.

# %%
sample = distNormal.getSample(size)
iterExtrema.increment(sample)

# %%
# We print the total number of iterations and the extrema.
print("Total number of iteration: " + str(iterExtrema.getIteration()))
print("Minimum: ", iterExtrema.getMin())
print("Maximum: ", iterExtrema.getMax())

otv.View.ShowAll()

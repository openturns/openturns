"""
Iterative mean
==============
"""

# %%
# In this example, we show how to use the :class:`~openturns.IterativeMoments` class. This algorithm computes an estimate of the mean using iterative algorithms. In other words, this algorithm can be used to iteratively update the estimate of the mean using one point at a time instead of using the full sample.

# %%
import openturns as ot
import openturns.viewer as otv

# %%
# We create the mean object.

# %%
dim = 1
myMean = ot.IterativeMoments(1, dim)

# %%
# We can now perform the simulations. In the following session, we increment using the `increment` method one `Point` at a time.
# The current mean is given by the `getMean` and the current number of iterations
# is given by the `getIteration` method.

# %%
n = ot.Normal(dim)
size = 200

data = ot.Sample()
for i in range(size):
    point = n.getRealization()
    myMean.increment(point)
    data.add(myMean.getMean())

# %%
# We display the evolution of the mean.
curve = ot.Curve(data)
graph = ot.Graph("Evolution of the mean", "iteration nb", "mean", True)
graph.add(curve)
view = otv.View(graph)

# %%
# We can also increment with a `Sample`.

# %%
sample = n.getSample(size)
myMean.increment(sample)

# %%
# We print the total number of iterations and the mean.
print ("Total number of iteration " + str(myMean.getIteration()))
print (myMean.getMean())

otv.View.ShowAll()

"""
Using iterative statistics
==========================
"""

# %%
# In this example, we use the :class:`~openturns.IterativeMoments` class to compute iterative statistics.
# This class stores centered moments up to a prescribed order iteratively. Then several statistics based on the moments are available depending on the chosen order.

# %%
import openturns as ot
import openturns.viewer as otv
ot.RandomGenerator.SetSeed(0)

# %%
# We first create a one-dimensional Gaussian random variable to generate data.
dim = 1
distNormal = ot.Normal(dim)

# %%
# Then we use the centered moments up to order 4 with the :class:`~openturns.IterativeMoments` class by giving the order (here 4) and the dimension (here 1):

# %%
order = 4
iterMoments = ot.IterativeMoments(order, dim)


# %%
# We can now perform the simulations.
# The :class:`~openturns.IterativeMoments` object stores the centered moments iteratively.
# We first increment the object with one :class:`~openturns.Point` at a time.
# At any given step the current mean is obtained thanks to the :meth:`~openturns.IterativeMoments.getMean` method and the
# current number of iterations is given by the :meth:`~openturns.IterativeMoments.getIteration` method.
size = 2000
meanEvolution = ot.Sample()
for i in range(size):
    point = distNormal.getRealization()
    iterMoments.increment(point)
    meanEvolution.add(iterMoments.getMean())

# %%
# We display the evolution of the mean.
curve = ot.Curve(meanEvolution)
graph = ot.Graph("Evolution of the mean", "iteration nb", "mean", True)
graph.add(curve)
view = otv.View(graph)

# %%
# We can also increment with a :class:`~openturns.Sample`.

# %%
sample = distNormal.getSample(size)
iterMoments.increment(sample)

# %%
# We print the total number of iterations and the mean.
print("Total number of iteration: " + str(iterMoments.getIteration()))
print("Mean: ", iterMoments.getMean())

# %%
# For the order of the `iterMoments` object is 4, we also have access to other statistics such as the variance (order 2), the skewness (order 3) or the kurtosis (order 4). For instance, a specified order of 3 would leave only the variance and the skewness available.
print("Variance: ", iterMoments.getVariance())
print("Skewness: ", iterMoments.getSkewness())
print("Kurtosis: ", iterMoments.getKurtosis())

otv.View.ShowAll()

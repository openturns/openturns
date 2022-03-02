"""
Iterative threshold exceedance
==============================
"""

# %%
# In this example, we use the :class:`~openturns.IterativeThresholdExceedance` class to count the number of threshold exceedance.

# %%
import openturns as ot
import openturns.viewer as otv
ot.RandomGenerator.SetSeed(0)

# %%
# We first create a one-dimensional Gaussian random variable to generate data.
dim = 1
distNormal = ot.Normal(dim)

# %%
# For illustration sake, let us consider a threshold value of 3.0. Each time the data is higher than 3.0 we count one exceedance. The counter used by the :class:`~openturns.IterativeThresholdExceedance` class is updated iteratively.

# %%
thresholdValue = 3.0
iterThreshold = ot.IterativeThresholdExceedance(dim, thresholdValue)

# %%
# We can now perform the simulations.
# In our case most of the data fall below the specified threshold value so the number of exceedance should be low.

# %%
# We first increment the object with one :class:`~openturns.Point` at a time.
# At any given step the current number of exceedance is obtained thanks to
# the :meth:`~openturns.IterativeThresholdExceedance.getThresholdExceedance()` method.

# %%
size = 5000
exceedanceNumbers = ot.Sample()
for i in range(size):
    point = distNormal.getRealization()
    iterThreshold.increment(point)
    exceedanceNumbers.add(iterThreshold.getThresholdExceedance())

# %%
# We display the evolution of the number of exceedance.

# %%
curve = ot.Curve(exceedanceNumbers)
curve.setColor("blue")
curve.setLegend("number of exceedance")

# %%
graph = ot.Graph(
    "Evolution of the number of exceedance",
    "iteration nb",
    "number of exceedance",
    True,
)
graph.add(curve)
graph.setLegends(["number of exceedance"])
graph.setLegendPosition("bottomright")
view = otv.View(graph)

# %%
# We can also increment with a :class:`~openturns.Sample`.

# %%
sample = distNormal.getSample(size)
iterThreshold.increment(sample)
print("Number of exceedance: ", iterThreshold.getThresholdExceedance())

# %%
# A simple computation gives the probability of the data to be higher than 3 is approximatively 0.0013 that we compare with empirical probability. We see the probalities are close.
print("Exceedance prb: ", 0.0013)
print(
    "Empirical exceedance prb: ",
    iterThreshold.getThresholdExceedance()[0] / iterThreshold.getIteration(),
)

otv.View.ShowAll()

"""
Iterative threshold exceedance
==============================
"""

# %%
# In this example, we use the :class:`~openturns.IterativeThresholdExceedance` class to count the number of threshold exceedances.

# %%
import openturns as ot
import openturns.viewer as otv

# %%
# We first create a one-dimensional Gaussian random variable to generate data.
dim = 1
distNormal = ot.Normal(dim)

# %%
# Let us consider a threshold value of 1.0. Each data value higher than 1.0 is counted as one exceedance. The counter used by the :class:`~openturns.IterativeThresholdExceedance` class is updated iteratively.

# %%
thresholdValue = 1.0
iterThreshold = ot.IterativeThresholdExceedance(dim, thresholdValue)

# %%
# We can now perform the simulations.
# In our case most of the data fall below the specified threshold value so the number of exceedances should be low.

# %%
# We first increment the object one :class:`~openturns.Point` at a time.
# At any given step the current number of exceedance is obtained with
# the :meth:`~openturns.IterativeThresholdExceedance.getThresholdExceedance()` method.

# %%
size = 5000
exceedanceNumbers = ot.Sample()
probabilityEstimateSample = ot.Sample()
for i in range(size):
    point = distNormal.getRealization()
    iterThreshold.increment(point)
    numberOfExceedances = iterThreshold.getThresholdExceedance()[0]
    exceedanceNumbers.add([numberOfExceedances])
    probabilityEstimate = numberOfExceedances / iterThreshold.getIterationNumber()
    probabilityEstimateSample.add([probabilityEstimate])

# %%
# We display the evolution of the number of exceedances.

# %%
curve = ot.Curve(exceedanceNumbers)
curve.setLegend("number of exceedance")
#
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
# We see that the probability of exceeding the threshold converges. 

# %%
iterationSample = ot.Sample.BuildFromPoint(range(1, size + 1))
curve = ot.Curve(iterationSample, probabilityEstimateSample)
curve.setLegend("Prob. of exceeding the threshold")
#
graph = ot.Graph(
    "Evolution of the sample probability",
    "iteration nb",
    "estimate of the probability",
    True,
)
graph.add(curve)
graph.setLegendPosition("topleft")
graph.setLogScale(ot.GraphImplementation.LOGX)
view = otv.View(graph)

# %%
# We can also increment with a :class:`~openturns.Sample`.

# %%
sample = distNormal.getSample(size)
iterThreshold.increment(sample)
numberOfExceedances = iterThreshold.getThresholdExceedance()[0]
print("Number of exceedance: ", numberOfExceedances)
# %%
# A simple computation shows that the probability of the data being higher than :math:`1` is :math:`0.1587` (with 4 significant digits).
# The empirical probability is close to this value:
distribution = ot.Normal()
exactProbability = distribution.computeComplementaryCDF(thresholdValue)
print("Exact probability: ", exactProbability)
print(
    "Empirical exceedance prb: ",
    iterThreshold.getThresholdExceedance()[0] / iterThreshold.getIterationNumber(),
)

otv.View.ShowAll()

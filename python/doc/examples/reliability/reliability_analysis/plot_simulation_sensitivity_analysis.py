"""
Estimate failure probability sensitivity with simulation
========================================================
"""

# %%
# In this example we estimate the sensitivity of a failure probability
# with respect to the parameters of the input distribution using the
# :class:`~openturns.SimulationSensitivityAnalysis` class.
#
# For a failure event :math:`\mathcal{F} = \{g(\vect{X}) > s\}` with
# input density :math:`f(\cdot; \theta)`, the sensitivity with respect
# to :math:`\theta` is:
#
# .. math::
#    \frac{\partial P_f}{\partial \theta}
#    = \mathbb{E}\left[\mathbf{1}_{\mathcal{F}}(\vect{X})
#    \frac{\partial \log f(\vect{X}; \theta)}{\partial \theta}\right].
#
# With Monte Carlo sampling this expectation is estimated by the empirical
# mean over the input sample. With importance sampling with importance
# density :math:`h`, each realization is weighted by the likelihood ratio
# :math:`w = f/h`:
#
# .. math::
#    \frac{\partial P_f}{\partial \theta}
#    \approx \frac{1}{n}\sum_{i=1}^n
#    \mathbf{1}_{\mathcal{F}}(\vect{X}_i)
#    \frac{\partial \log f(\vect{X}_i; \theta)}{\partial \theta} w_i.
#
# We first run a Monte Carlo reference, then importance sampling in the
# physical space, and finally importance sampling in the standard space
# mapped back to the physical space.

# %%
import openturns as ot
import openturns.viewer as otv

ot.RandomGenerator.SetSeed(0)

# %%
# Define the model
# -----------------
# A linear limit-state with Gaussian inputs.

# %%
distribution = ot.Normal([0.0] * 2, [1.0] * 2)
model = ot.SymbolicFunction(["x0", "x1"], ["x0 + 2.0 * x1"])
threshold = 3.0

# %%
# Monte Carlo reference
# ---------------------
# With uniform weights the samples stored by the memoized model are enough.

# %%
memo_mc = ot.MemoizeFunction(model)
event_mc = ot.ThresholdEvent(
    ot.CompositeRandomVector(memo_mc, ot.RandomVector(distribution)),
    ot.Greater(),
    threshold,
)
algo_mc = ot.ProbabilitySimulationAlgorithm(event_mc)
algo_mc.setMaximumOuterSampling(100)
algo_mc.setBlockSize(500)
algo_mc.setMaximumCoefficientOfVariation(0.05)
algo_mc.run()
result_mc = algo_mc.getResult()
print("Monte Carlo Pf =", result_mc.getProbabilityEstimate())

sensitivity_mc = ot.SimulationSensitivityAnalysis(result_mc)
print("Monte Carlo sensitivity =", sensitivity_mc.computeEventProbabilitySensitivity())
print("Monte Carlo mean point =", sensitivity_mc.computeMeanPointInEventDomain())

graph = sensitivity_mc.drawImportanceFactors()
view = otv.View(graph)

# %%
# Importance sampling in the physical space
# -----------------------------------------
# The importance distribution is centered on the failure domain.
# The likelihood ratio must be passed as weights, otherwise the
# sensitivity is biased.

# %%
memo_is = ot.MemoizeFunction(model)
event_is = ot.ThresholdEvent(
    ot.CompositeRandomVector(memo_is, ot.RandomVector(distribution)),
    ot.Greater(),
    threshold,
)
importance = ot.Normal([1.0, 2.0], [1.0] * 2)
experiment = ot.ImportanceSamplingExperiment(importance)
algo_is = ot.ProbabilitySimulationAlgorithm(event_is, experiment)
algo_is.setMaximumOuterSampling(100)
algo_is.setBlockSize(500)
algo_is.setMaximumCoefficientOfVariation(0.05)
algo_is.run()
print("Importance sampling Pf =", algo_is.getResult().getProbabilityEstimate())

input_sample = memo_is.getInputHistory()
output_sample = memo_is.getOutputHistory()
weights = [
    distribution.computePDF(point) / importance.computePDF(point)
    for point in input_sample
]

sensitivity_is = ot.SimulationSensitivityAnalysis(
    event_is, input_sample, output_sample, weights
)
print("Importance sampling sensitivity =", sensitivity_is.computeEventProbabilitySensitivity())
print("Importance sampling mean point =", sensitivity_is.computeMeanPointInEventDomain())

# %%
# Importance sampling in the standard space
# -----------------------------------------
# In practice importance sampling is often performed in the standard space
# through a :class:`~openturns.StandardEvent`. The sensitivity with respect
# to the physical distribution is then recovered by mapping the standard
# input sample back to the physical space and weighting by the ratio of the
# standard density over the importance density.

# %%
memo_phys = ot.MemoizeFunction(model)
phys_event = ot.ThresholdEvent(
    ot.CompositeRandomVector(memo_phys, ot.RandomVector(distribution)),
    ot.Greater(),
    threshold,
)
std_event = ot.StandardEvent(phys_event)
memo_std = ot.MemoizeFunction(std_event.getFunction())
std_event_is = ot.ThresholdEvent(
    ot.CompositeRandomVector(memo_std, std_event.getAntecedent()),
    std_event.getOperator(),
    std_event.getThreshold(),
)
std_importance = ot.Normal([1.0, 2.0], ot.CovarianceMatrix(2))
std_experiment = ot.ImportanceSamplingExperiment(std_importance)
std_algo = ot.ProbabilitySimulationAlgorithm(std_event_is, std_experiment)
std_algo.setMaximumOuterSampling(100)
std_algo.setBlockSize(500)
std_algo.setMaximumCoefficientOfVariation(0.05)
std_algo.run()
print("Standard-space Pf =", std_algo.getResult().getProbabilityEstimate())

std_input = memo_std.getInputHistory()
std_output = memo_std.getOutputHistory()
std_distribution = ot.Normal(2)
std_weights = [
    std_distribution.computePDF(point) / std_importance.computePDF(point)
    for point in std_input
]
phys_input = distribution.getInverseIsoProbabilisticTransformation()(std_input)

sensitivity_std = ot.SimulationSensitivityAnalysis(
    phys_event, phys_input, std_output, std_weights
)
print(
    "Standard-space sensitivity w.r.t. physical parameters =",
    sensitivity_std.computeEventProbabilitySensitivity(),
)

# %%
otv.View.ShowAll()

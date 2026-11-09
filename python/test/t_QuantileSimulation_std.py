#!/usr/bin/env python

import bisect
import math
import os

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()


def sample_values(sample):
    return [sample[i, 0] for i in range(sample.getSize())]


def weighted_quantile(values, weights, alpha):
    """Independent lower weighted empirical inverse."""
    total = sum(weights)
    pairs = sorted(zip(values, weights))
    cumulative = 0.0
    for value, weight in pairs:
        cumulative += weight / total
        if cumulative >= alpha:
            return value
    return pairs[-1][0]


def weighted_woodruff(values, weights, alpha, beta):
    """Independent weighted Woodruff interval and standard error."""
    total = sum(weights)
    normalized = [weight / total for weight in weights]
    pairs = sorted(zip(values, normalized))
    ys = [pair[0] for pair in pairs]
    ws = [pair[1] for pair in pairs]
    estimate = weighted_quantile(values, weights, alpha)
    cdf = sum(wi for yi, wi in zip(ys, ws) if yi <= estimate)
    low_square = sum(wi * wi for yi, wi in zip(ys, ws) if yi <= estimate)
    high_square = sum(wi * wi for yi, wi in zip(ys, ws) if yi > estimate)
    variance = (1.0 - cdf) ** 2 * low_square + cdf ** 2 * high_square
    z = ot.Normal().computeQuantile(0.5 * (1.0 + beta))[0]
    half_width = z * math.sqrt(variance)
    lower = weighted_quantile(values, weights, alpha - half_width)
    upper = weighted_quantile(values, weights, alpha + half_width)
    lower = min(lower, estimate)
    upper = max(upper, estimate)
    return lower, upper, (upper - lower) / (2.0 * z)


def binomial_coverage(size, alpha, rank1, rank2):
    """P(rank1 + 1 <= B <= rank2) for B ~ Binomial(size, alpha).

    The ranks are zero-based order-statistic indices, so the event that
    the interval [Y_(rank1), Y_(rank2)] covers the quantile is
    rank1 + 1 <= B <= rank2.
    """
    return sum(
        math.comb(size, k) * alpha ** k * (1.0 - alpha) ** (size - k)
        for k in range(rank1 + 1, rank2 + 1)
    )


COVERAGE_EXACT = otexp.QuantileSimulationResultImplementation.COVERAGE_EXACT
COVERAGE_ASYMPTOTIC = otexp.QuantileSimulationResultImplementation.COVERAGE_ASYMPTOTIC
COVERAGE_ANYTIME_VALID = otexp.QuantileSimulationResultImplementation.COVERAGE_ANYTIME_VALID
COVERAGE_NONE = otexp.QuantileSimulationResultImplementation.COVERAGE_NONE
EMPIRICAL_INVERSE = otexp.QuantileSimulationResultImplementation.EMPIRICAL_INVERSE
INTERPOLATED = otexp.QuantileSimulationResultImplementation.INTERPOLATED

# ---------------------------------------------------------------------
# 1. Known-answer benchmark: deterministic weighted sample
# ---------------------------------------------------------------------
values = [0.0, 1.0, 2.0, 3.0, 4.0]
weights = [1.0, 2.0, 3.0, 2.0, 2.0]
sample = ot.Sample([[value] for value in values])
fixed = ot.FixedExperiment(sample, weights)
output = ot.RandomVector(ot.Normal())
algo = otexp.QuantileSimulationAlgorithm(output, fixed, 0.5, 0.95)
algo.setKeepSample(True)
algo.run()
result = algo.getResult()

assert result.getQuantileEstimate() == 2.0, "weighted median"
assert result.getQuantileLevel() == 0.5
assert result.getConfidenceLevel() == 0.95
assert result.getEstimatorType() == EMPIRICAL_INVERSE
assert result.getCoverageType() == COVERAGE_NONE
assert result.getSampleSize() == len(values)
assert result.getEffectiveSampleSize() == 1.0 / sum((w / sum(weights)) ** 2 for w in weights)
assert result.getConvergenceHistory().getSize() == 1
assert result.isFixedSample()

# The retained sample is the fixed design, and the weights are the raw ones.
assert sample_values(algo.getOutputSample()) == values
assert [algo.getWeights()[i] for i in range(len(values))] == weights

# Weighted Woodruff interval, independently recomputed.
expected_lower, expected_upper, expected_error = weighted_woodruff(values, weights, 0.5, 0.95)
interval = result.getConfidenceInterval()
ott.assert_almost_equal(interval.getLowerBound()[0], expected_lower, 1.0e-12, 1.0e-12)
ott.assert_almost_equal(interval.getUpperBound()[0], expected_upper, 1.0e-12, 1.0e-12)
ott.assert_almost_equal(result.getStandardError(), expected_error, 1.0e-12, 1.0e-12)
assert interval.getLowerBound()[0] <= result.getQuantileEstimate() <= interval.getUpperBound()[0]

# A larger alpha selects a larger order statistic.
algo.setAlpha(0.95)
algo.run()
assert algo.getResult().getQuantileEstimate() == 4.0, "weighted upper quantile"

# The interpolated estimator is dispatched and reported.
algo.setAlpha(0.5)
algo.setEstimatorType(INTERPOLATED)
algo.run()
result = algo.getResult()
assert result.getEstimatorType() == INTERPOLATED
assert result.getCoverageType() == COVERAGE_NONE
ott.assert_almost_equal(result.getQuantileEstimate(), 5.0 / 3.0, 1.0e-12, 1.0e-12)

# ---------------------------------------------------------------------
# 2. Exact i.i.d. Monte Carlo path and binomial oracle
# ---------------------------------------------------------------------
ot.RandomGenerator.SetSeed(12345)
alpha = 0.95
beta = 0.95
size = 1000
algo = otexp.QuantileSimulationAlgorithm(output, alpha, beta)
algo.setSampleSize(size)
algo.setKeepSample(True)
algo.run()
result = algo.getResult()

assert result.getCoverageType() == COVERAGE_EXACT
assert result.getEstimatorType() == EMPIRICAL_INVERSE
assert result.getSampleSize() == size
assert result.getEffectiveSampleSize() == float(size)
assert result.getOuterSampling() == 1

retained = sorted(sample_values(algo.getOutputSample()))
expected_estimate = retained[math.ceil(size * alpha) - 1]
assert result.getQuantileEstimate() == expected_estimate, "exact empirical inverse"

# The interval bounds are order statistics of the retained sample.
lower = result.getConfidenceInterval().getLowerBound()[0]
upper = result.getConfidenceInterval().getUpperBound()[0]
assert lower in retained
assert upper in retained
rank1 = bisect.bisect_left(retained, lower)
rank2 = bisect.bisect_right(retained, upper) - 1
assert rank1 <= rank2
assert retained[rank1] == lower
assert retained[rank2] == upper
coverage = binomial_coverage(size, alpha, rank1, rank2)
assert coverage >= beta - 1.0e-12, "exact interval coverage"
assert coverage <= beta + 0.01, "exact interval is not needlessly conservative"
assert lower <= result.getQuantileEstimate() <= upper

# ---------------------------------------------------------------------
# 3. Interpolated estimator on an i.i.d. sample
# ---------------------------------------------------------------------
ot.RandomGenerator.SetSeed(12345)
algo = otexp.QuantileSimulationAlgorithm(output, alpha, beta)
algo.setSampleSize(size)
algo.setEstimatorType(INTERPOLATED)
algo.setKeepSample(True)
algo.run()
result = algo.getResult()
assert result.getEstimatorType() == INTERPOLATED
assert result.getCoverageType() == COVERAGE_ASYMPTOTIC
retained = sorted(sample_values(algo.getOutputSample()))
scalar_index = alpha * size - 0.5
index = int(math.floor(scalar_index))
fraction = scalar_index - index
expected = (1.0 - fraction) * retained[index] + fraction * retained[index + 1]
ott.assert_almost_equal(result.getQuantileEstimate(), expected, 1.0e-12, 1.0e-12)

# ---------------------------------------------------------------------
# 4. Experiment dispatch: LHS, randomized QMC, deterministic QMC, IS
# ---------------------------------------------------------------------
distribution = ot.Normal()

# Latin hypercube: independent randomized replicates, asymptotic label.
lhs = ot.LHSExperiment(distribution, 100, True, True)
algo = otexp.QuantileSimulationAlgorithm(output, lhs, alpha, beta)
algo.setReplicateCount(20)
algo.run()
result = algo.getResult()
assert result.getCoverageType() == COVERAGE_ASYMPTOTIC
assert result.getSampleSize() == 20 * 100
assert result.getEffectiveSampleSize() == 20 * 100.0
assert result.getOuterSampling() == 20
assert result.getConvergenceHistory().getSize() == 20
assert result.getConfidenceInterval().getLowerBound()[0] <= result.getQuantileEstimate()
assert result.getQuantileEstimate() <= result.getConfidenceInterval().getUpperBound()[0]

# A Latin hypercube that reuses its shuffle gives correlated replicates:
# the replicate interval is rejected instead of reporting a false coverage.
algo = otexp.QuantileSimulationAlgorithm(output, ot.LHSExperiment(distribution, 100), alpha, beta)
with ott.assert_raises(TypeError):
    algo.run()

# A Sobol wrapper hides the underlying Latin hypercube from the direct
# check.  A fixed random shift still reuses the design across replicates,
# so the replicate interval must be rejected as well.
sobol_lhs = ot.SobolIndicesExperiment(
    ot.LHSExperiment(distribution, 100, True, False), False
)
algo = otexp.QuantileSimulationAlgorithm(output, sobol_lhs, alpha, beta)
with ott.assert_raises(TypeError):
    algo.run()

# Deterministic quasi-Monte Carlo: no coverage guarantee.
qmc = ot.LowDiscrepancyExperiment(ot.SobolSequence(), distribution, 1024, False)
algo = otexp.QuantileSimulationAlgorithm(output, qmc, alpha, beta)
algo.run()
result = algo.getResult()
assert result.getCoverageType() == COVERAGE_NONE
assert result.getSampleSize() == 1024

# Deterministic quadrature rule: non-uniform positive weights, no coverage.
uniform_output = ot.RandomVector(ot.Uniform(0.0, 1.0))
quadrature = ot.GaussProductExperiment(ot.Uniform(0.0, 1.0), [7])
algo = otexp.QuantileSimulationAlgorithm(uniform_output, quadrature, 0.5, 0.95)
algo.run()
result = algo.getResult()
assert result.getCoverageType() == COVERAGE_NONE
assert result.getSampleSize() == 7
ott.assert_almost_equal(result.getQuantileEstimate(), 0.5, 1.0e-12, 1.0e-12)

# Randomized quasi-Monte Carlo: independent randomizations, asymptotic label.
qmc = ot.LowDiscrepancyExperiment(ot.SobolSequence(), distribution, 1024, False)
qmc.setRandomize(True)
algo = otexp.QuantileSimulationAlgorithm(output, qmc, alpha, beta)
algo.setReplicateCount(10)
algo.run()
result = algo.getResult()
assert result.getCoverageType() == COVERAGE_ASYMPTOTIC
assert result.getSampleSize() == 10 * 1024

# Importance sampling: weighted Woodruff interval and effective sample size.
importance = ot.ImportanceSamplingExperiment(distribution, ot.Normal(1.0, 1.0), 2000)
algo = otexp.QuantileSimulationAlgorithm(output, importance, alpha, beta)
algo.run()
result = algo.getResult()
assert result.getCoverageType() == COVERAGE_ASYMPTOTIC
assert result.getSampleSize() == 2000
assert result.getEffectiveSampleSize() < 2000.0
assert result.getEffectiveSampleSize() > 0.0

# ---------------------------------------------------------------------
# 5. Negative limit tests
# ---------------------------------------------------------------------
# A required exact coverage cannot be satisfied by a deterministic design.
algo = otexp.QuantileSimulationAlgorithm(output, fixed, 0.5, 0.95)
algo.setRequiredCoverage(COVERAGE_EXACT)
with ott.assert_raises(TypeError):
    algo.run()

# A required exact coverage is satisfied by the i.i.d. Monte Carlo path.
algo = otexp.QuantileSimulationAlgorithm(output, alpha, beta)
algo.setSampleSize(1000)
algo.setRequiredCoverage(COVERAGE_EXACT)
algo.run()
assert algo.getResult().getCoverageType() == COVERAGE_EXACT

# A multivariate output is rejected at construction time.
with ott.assert_raises(TypeError):
    otexp.QuantileSimulationAlgorithm(ot.RandomVector(ot.Normal(2)), alpha, beta)

# A multivariate output is also rejected by the result type.
with ott.assert_raises(TypeError):
    otexp.QuantileSimulationResultImplementation(ot.RandomVector(ot.Normal(2)), alpha, beta)

# Invalid levels are rejected.
with ott.assert_raises(TypeError):
    otexp.QuantileSimulationAlgorithm(output, 0.0, beta)
with ott.assert_raises(TypeError):
    otexp.QuantileSimulationAlgorithm(output, alpha, 1.0)

# The sample size must be positive and set before run.
algo = otexp.QuantileSimulationAlgorithm(output, alpha, beta)
with ott.assert_raises(TypeError):
    algo.run()
with ott.assert_raises(TypeError):
    algo.setSampleSize(0)

# The replicate count must be at least two.
algo = otexp.QuantileSimulationAlgorithm(output, lhs, alpha, beta)
with ott.assert_raises(TypeError):
    algo.setReplicateCount(1)

# The default replicate count comes from the ResourceMap.
default_count = ot.ResourceMap.GetAsUnsignedInteger(
    "QuantileSimulationAlgorithm-DefaultReplicateCount")
assert default_count == 30
ot.ResourceMap.SetAsUnsignedInteger("QuantileSimulationAlgorithm-DefaultReplicateCount", 7)
try:
    assert otexp.QuantileSimulationAlgorithm(output, lhs, alpha, beta).getReplicateCount() == 7
finally:
    ot.ResourceMap.SetAsUnsignedInteger(
        "QuantileSimulationAlgorithm-DefaultReplicateCount", default_count)

# An experiment whose distribution has the wrong dimension is rejected.
wrong = ot.LHSExperiment(ot.Normal(2), 100)
algo = otexp.QuantileSimulationAlgorithm(output, wrong, alpha, beta)
with ott.assert_raises(TypeError):
    algo.run()

# ---------------------------------------------------------------------
# 6. Reproducibility
# ---------------------------------------------------------------------
ot.RandomGenerator.SetSeed(54321)
algo1 = otexp.QuantileSimulationAlgorithm(output, alpha, beta)
algo1.setSampleSize(500)
algo1.run()
first = algo1.getResult()
ot.RandomGenerator.SetSeed(54321)
algo2 = otexp.QuantileSimulationAlgorithm(output, alpha, beta)
algo2.setSampleSize(500)
algo2.run()
second = algo2.getResult()
assert first.getQuantileEstimate() == second.getQuantileEstimate()
assert first.getConfidenceInterval() == second.getConfidenceInterval()

# A different seed gives a different sample and, with probability one,
# a different estimate.
ot.RandomGenerator.SetSeed(54322)
algo3 = otexp.QuantileSimulationAlgorithm(output, alpha, beta)
algo3.setSampleSize(500)
algo3.run()
assert algo3.getResult().getQuantileEstimate() != first.getQuantileEstimate()

# ---------------------------------------------------------------------
# 7. SWIG handle, downcast and persistence
# ---------------------------------------------------------------------
ot.RandomGenerator.SetSeed(7)
algo = otexp.QuantileSimulationAlgorithm(output, alpha, beta)
algo.setSampleSize(300)
algo.run()
result = algo.getResult()
assert isinstance(result, otexp.QuantileSimulationResult), "result handle type"

# The generic handle wraps the concrete algorithm and returns the same result.
handle = otexp.QuantileSimulation(algo)
assert isinstance(handle, otexp.QuantileSimulation)
assert handle.getSampleSize() == 300
assert handle.getResult().getQuantileEstimate() == result.getQuantileEstimate()

# The result can be copied into another handle through the implementation.
implementation = otexp.QuantileSimulationResultImplementation()
implementation.setOutput(output)
implementation.setQuantileLevel(alpha)
implementation.setConfidenceLevel(beta)
implementation.setQuantileEstimate(1.25)
implementation.setStandardError(0.5)
implementation.setConfidenceInterval(ot.Interval(0.25, 2.25))
implementation.setEstimatorType(EMPIRICAL_INVERSE)
implementation.setCoverageType(COVERAGE_EXACT)
implementation.setSampleSize(100)
implementation.setEffectiveSampleSize(100.0)
copied = otexp.QuantileSimulationResult(implementation)
assert copied.getQuantileEstimate() == 1.25
assert copied.getCoverageType() == COVERAGE_EXACT
assert copied.getEstimatorDescription() == "Empirical inverse"
assert copied.getCoverageDescription() == "Exact"
assert copied.drawQuantileConvergence().getDrawables().getSize() == 0
assert result.drawQuantileConvergence().getDrawables().getSize() >= 1

# The algorithm and the result survive an XML round trip.
path = "quantile_simulation_result.xml"
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager(path))
study.add("result", result)
study.add("algo", algo)
study.save()

study = ot.Study()
study.setStorageManager(ot.XMLStorageManager(path))
study.load()
loaded_result = otexp.QuantileSimulationResult()
study.fillObject("result", loaded_result)
loaded_algo = otexp.QuantileSimulationAlgorithm()
study.fillObject("algo", loaded_algo)
assert loaded_result.getQuantileEstimate() == result.getQuantileEstimate()
assert loaded_result.getConfidenceInterval() == result.getConfidenceInterval()
assert loaded_result.getCoverageType() == result.getCoverageType()
assert loaded_algo.getSampleSize() == 300
assert loaded_algo.getAlpha() == alpha
assert loaded_algo.getResult().getQuantileEstimate() == result.getQuantileEstimate()
os.remove(path)

# ---------------------------------------------------------------------
# 8. Result accessors, descriptions, graph and copy-on-write
# ---------------------------------------------------------------------
r_impl = otexp.QuantileSimulationResultImplementation()
assert r_impl.getOutput().getDimension() == 1
assert not r_impl.hasMasterSeed()
assert not r_impl.hasBiasEstimate()
assert r_impl.isFixedSample()
assert r_impl.getSampleSize() == 0
assert r_impl.getConfidenceInterval().getDimension() == 0

r_impl.setOutput(output)
r_impl.setQuantileLevel(0.9)
r_impl.setConfidenceLevel(0.8)
r_impl.setMasterSeed(1234)
assert r_impl.hasMasterSeed()
assert r_impl.getMasterSeed() == 1234
r_impl.setQuantileEstimate(-1.5)
r_impl.setStandardError(0.25)
r_impl.setConfidenceInterval(ot.Interval(-2.0, -1.0))
r_impl.setEstimatorType(INTERPOLATED)
r_impl.setCoverageType(COVERAGE_ANYTIME_VALID)
r_impl.setRequiredCoverage(COVERAGE_ASYMPTOTIC)
r_impl.setEffectiveSampleSize(12.5)
r_impl.setSampleSize(20)
r_impl.setBlockLength(4)
r_impl.setBiasEstimate(0.125)
r_impl.setIsFixedSample(False)
r_impl.setConvergenceHistory(ot.Sample([[20.0, -1.5, -2.0, -1.0]]))

assert r_impl.getOutput() == output
assert r_impl.getQuantileLevel() == 0.9
assert r_impl.getConfidenceLevel() == 0.8
assert r_impl.getQuantileEstimate() == -1.5
assert r_impl.getStandardError() == 0.25
assert r_impl.getConfidenceInterval() == ot.Interval(-2.0, -1.0)
assert r_impl.getEstimatorType() == INTERPOLATED
assert r_impl.getCoverageType() == COVERAGE_ANYTIME_VALID
assert r_impl.getRequiredCoverage() == COVERAGE_ASYMPTOTIC
assert r_impl.getEffectiveSampleSize() == 12.5
assert r_impl.getSampleSize() == 20
assert r_impl.getBlockLength() == 4
assert r_impl.hasBiasEstimate()
assert r_impl.getBiasEstimate() == 0.125
assert not r_impl.isFixedSample()
assert r_impl.getEstimatorDescription() == "Interpolated"
assert r_impl.getCoverageDescription() == "Anytime-valid"
assert r_impl.drawQuantileConvergence().getDrawables().getSize() == 3
assert "QuantileSimulationResultImplementation" in repr(r_impl)
assert "quantile estimate" in str(r_impl)

for estimator, label in [(EMPIRICAL_INVERSE, "Empirical inverse"),
                         (INTERPOLATED, "Interpolated")]:
    r_impl.setEstimatorType(estimator)
    assert r_impl.getEstimatorDescription() == label
for coverage, label in [(COVERAGE_EXACT, "Exact"),
                        (COVERAGE_ASYMPTOTIC, "Asymptotic"),
                        (COVERAGE_ANYTIME_VALID, "Anytime-valid"),
                        (COVERAGE_NONE, "None")]:
    r_impl.setCoverageType(coverage)
    assert r_impl.getCoverageDescription() == label

# A two-column history has no confidence bounds and yields one curve.
r_impl.setConvergenceHistory(ot.Sample([[10.0, -1.0], [20.0, -1.5]]))
assert r_impl.drawQuantileConvergence().getDrawables().getSize() == 1

r_handle = otexp.QuantileSimulationResult(r_impl)
assert r_handle.getOutput() == output
assert r_handle.getQuantileEstimate() == -1.5
assert r_handle.getCoverageType() == COVERAGE_NONE

r_handle.setOutput(output)
r_handle.setQuantileLevel(0.75)
r_handle.setConfidenceLevel(0.7)
r_handle.setMasterSeed(42)
r_handle.setQuantileEstimate(2.5)
r_handle.setStandardError(0.1)
r_handle.setConfidenceInterval(ot.Interval(2.0, 3.0))
r_handle.setEstimatorType(EMPIRICAL_INVERSE)
r_handle.setCoverageType(COVERAGE_ASYMPTOTIC)
r_handle.setRequiredCoverage(COVERAGE_EXACT)
r_handle.setEffectiveSampleSize(15.0)
r_handle.setSampleSize(30)
r_handle.setBlockLength(3)
r_handle.setBiasEstimate(-0.1)
r_handle.setIsFixedSample(True)
r_handle.setConvergenceHistory(ot.Sample([[10.0, 2.0], [20.0, 2.5]]))
r_handle.setOuterSampling(2)
r_handle.setBlockSize(5)
r_handle.setTimeDuration(0.125)

assert r_handle.getQuantileLevel() == 0.75
assert r_handle.getConfidenceLevel() == 0.7
assert r_handle.hasMasterSeed()
assert r_handle.getMasterSeed() == 42
assert r_handle.getStandardError() == 0.1
assert r_handle.getConfidenceInterval() == ot.Interval(2.0, 3.0)
assert r_handle.getEstimatorType() == EMPIRICAL_INVERSE
assert r_handle.getRequiredCoverage() == COVERAGE_EXACT
assert r_handle.getEffectiveSampleSize() == 15.0
assert r_handle.getSampleSize() == 30
assert r_handle.getBlockLength() == 3
assert r_handle.hasBiasEstimate()
assert r_handle.getBiasEstimate() == -0.1
assert r_handle.isFixedSample()
assert r_handle.getOuterSampling() == 2
assert r_handle.getBlockSize() == 5
assert r_handle.getTimeDuration() == 0.125
assert r_handle.drawQuantileConvergence().getDrawables().getSize() == 1
assert r_handle.getEstimatorDescription() == "Empirical inverse"
assert r_handle.getCoverageDescription() == "Asymptotic"
assert "quantile estimate" in str(r_handle)

# The copy shares the implementation until the first mutation.
r_copy = otexp.QuantileSimulationResult(r_handle)
r_copy.setQuantileEstimate(9.0)
assert r_handle.getQuantileEstimate() == 2.5
assert r_copy.getQuantileEstimate() == 9.0
assert r_copy.getSampleSize() == 30

with ott.assert_raises(TypeError):
    r_impl.setQuantileLevel(0.0)
with ott.assert_raises(TypeError):
    r_impl.setConfidenceLevel(1.0)
with ott.assert_raises(TypeError):
    r_impl.setOutput(ot.RandomVector(ot.Normal(2)))
with ott.assert_raises(TypeError):
    r_impl.setQuantileEstimate(float("inf"))
with ott.assert_raises(TypeError):
    r_impl.setStandardError(-1.0)
with ott.assert_raises(TypeError):
    r_impl.setConfidenceInterval(ot.Interval([0.0, 0.0], [1.0, 1.0]))
with ott.assert_raises(TypeError):
    r_impl.setEstimatorType(99)
with ott.assert_raises(TypeError):
    r_impl.setCoverageType(99)
with ott.assert_raises(TypeError):
    r_impl.setRequiredCoverage(99)
with ott.assert_raises(TypeError):
    r_impl.setEffectiveSampleSize(0.0)
with ott.assert_raises(TypeError):
    r_impl.setSampleSize(0)
with ott.assert_raises(TypeError):
    r_impl.setBiasEstimate(float("nan"))
with ott.assert_raises(TypeError):
    r_impl.setConvergenceHistory(ot.Sample([[1.0], [2.0]]))

# ---------------------------------------------------------------------
# 9. Algorithm accessors, base class and handle
# ---------------------------------------------------------------------
algo = otexp.QuantileSimulationAlgorithm(output)
assert algo.getAlpha() == 0.95
assert algo.getConfidenceLevel() == 0.95
assert not algo.hasExperiment()
assert algo.getReplicateCount() == 30
assert not algo.getKeepSample()
assert algo.getSampleSize() == 0
assert algo.getInputSample().getSize() == 0
assert algo.getOutputSample().getSize() == 0
assert algo.getWeights().getSize() == 0

algo.setOutput(output)
algo.setAlpha(0.6)
algo.setConfidenceLevel(0.7)
algo.setEstimatorType(INTERPOLATED)
algo.setRequiredCoverage(COVERAGE_NONE)
algo.setReplicateCount(7)
assert algo.getAlpha() == 0.6
assert algo.getConfidenceLevel() == 0.7
assert algo.getEstimatorType() == INTERPOLATED
assert algo.getRequiredCoverage() == COVERAGE_NONE
assert algo.getReplicateCount() == 7

algo.setBlockSize(4)
algo.setSampleSize(10)
assert algo.getSampleSize() == 10
assert algo.getMaximumOuterSampling() == 3

mce = ot.MonteCarloExperiment(output.getDistribution(), 40)
algo.setExperiment(mce)
assert algo.hasExperiment()
assert algo.getSampleSize() == 40
assert algo.getExperiment().getSize() == 40
algo.setSampleSize(50)
assert algo.getSampleSize() == 50
assert algo.getExperiment().getSize() == 50

algo.setAlpha(0.5)
algo.setConfidenceLevel(0.5)
algo.setEstimatorType(EMPIRICAL_INVERSE)
algo.setSampleSize(100)
algo.setKeepSample(True)
ot.RandomGenerator.SetSeed(11)
algo.run()
assert algo.getInputSample().getSize() == 100
assert algo.getOutputSample().getSize() == 100
assert algo.getWeights().getSize() == 100
algo.setKeepSample(False)
assert algo.getInputSample().getSize() == 0
assert algo.getOutputSample().getSize() == 0
assert algo.getWeights().getSize() == 0

# A composite output without an explicit experiment uses the antecedent
# distribution to build the default Monte Carlo experiment.
base_vector = ot.RandomVector(ot.Normal())
composite = ot.CompositeRandomVector(
    ot.SymbolicFunction(["x"], ["x^2"]), base_vector)
algo = otexp.QuantileSimulationAlgorithm(composite, 0.5, 0.5)
algo.setSampleSize(200)
algo.run()
assert algo.getResult().getQuantileEstimate() >= 0.0
assert algo.getResult().getCoverageType() == COVERAGE_EXACT

# The base implementation validates the inputs then raises from runFixedSample.
base = otexp.QuantileSimulationImplementation(output, 0.5, 0.5)
base.setSampleSize(10)
with ott.assert_raises(RuntimeError):
    base.run()
base.setAlpha(0.6)
base.setConfidenceLevel(0.7)
base.setEstimatorType(INTERPOLATED)
base.setRequiredCoverage(COVERAGE_EXACT)
base.setOutput(output)
assert base.getAlpha() == 0.6
assert base.getConfidenceLevel() == 0.7
assert base.getEstimatorType() == INTERPOLATED
assert base.getRequiredCoverage() == COVERAGE_EXACT
base.setResult(r_handle)
assert base.getResult().getQuantileEstimate() == 2.5

# The standard handle wraps the base implementation, which has no sampler.
standard_handle = otexp.QuantileSimulation(output, 0.5, 0.5)
assert standard_handle.getOutput() == output
assert standard_handle.getAlpha() == 0.5
assert standard_handle.getConfidenceLevel() == 0.5

# The handle forwards all accessors and can be copied with copy-on-write.
handle_algo = otexp.QuantileSimulationAlgorithm(output, 0.5, 0.5)
handle_algo.setSampleSize(100)
handle = otexp.QuantileSimulation(handle_algo)
handle.setSampleSize(100)
handle.setEstimatorType(EMPIRICAL_INVERSE)
handle.setRequiredCoverage(COVERAGE_ASYMPTOTIC)
handle.setMaximumOuterSampling(9)
handle.setMaximumCoefficientOfVariation(0.3)
handle.setMaximumStandardDeviation(0.4)
handle.setBlockSize(5)
handle.setMaximumTimeDuration(12.0)
handle.setConvergenceStrategy(ot.Full())
assert handle.getOutput() == output
assert handle.getAlpha() == 0.5
assert handle.getConfidenceLevel() == 0.5
assert handle.getSampleSize() == 100
assert handle.getEstimatorType() == EMPIRICAL_INVERSE
assert handle.getRequiredCoverage() == COVERAGE_ASYMPTOTIC
assert handle.getMaximumOuterSampling() == 9
assert handle.getMaximumCoefficientOfVariation() == 0.3
assert handle.getMaximumStandardDeviation() == 0.4
assert handle.getBlockSize() == 5
assert handle.getMaximumTimeDuration() == 12.0
assert str(handle.getConvergenceStrategy()) == str(ot.Full())

progress_calls = []
handle.setProgressCallback(lambda percent: progress_calls.append(percent))
handle.setStopCallback(lambda: False)
with ott.assert_raises(TypeError):
    handle.setProgressCallback(17)
with ott.assert_raises(TypeError):
    handle.setStopCallback(17)
# Passing None clears the callback, as in the other simulation algorithms.
handle.setProgressCallback(None)
handle.setStopCallback(None)

ot.RandomGenerator.SetSeed(12)
handle.run()
hresult = handle.getResult()
assert hresult.getCoverageType() == COVERAGE_EXACT
assert progress_calls == [], "callbacks are not invoked by the fixed-sample algorithm"

handle_copy = otexp.QuantileSimulation(handle)
handle_copy.setAlpha(0.4)
assert handle.getAlpha() == 0.5
assert handle_copy.getAlpha() == 0.4

default_handle = otexp.QuantileSimulation()
default_handle.setSampleSize(5)
with ott.assert_raises(RuntimeError):
    default_handle.run()

# ---------------------------------------------------------------------
# 10. Interpolation boundaries, zero weights and constant samples
# ---------------------------------------------------------------------
values3 = [0.0, 10.0, 20.0]
weights3 = [1.0, 2.0, 1.0]
fixed3 = ot.FixedExperiment(
    ot.Sample([[value] for value in values3]), ot.Point(weights3))
algo = otexp.QuantileSimulationAlgorithm(output, fixed3, 0.5, 0.95)

for level, empirical, interpolated in [
        (0.25, 0.0, 0.0),
        (0.5, 10.0, 5.0),
        (0.75, 10.0, 10.0),
        (0.99, 20.0, 19.6)]:
    algo.setAlpha(level)
    algo.setEstimatorType(EMPIRICAL_INVERSE)
    algo.run()
    ott.assert_almost_equal(
        algo.getResult().getQuantileEstimate(), empirical, 1.0e-12, 1.0e-12)
    algo.setEstimatorType(INTERPOLATED)
    algo.run()
    ott.assert_almost_equal(
        algo.getResult().getQuantileEstimate(), interpolated, 1.0e-12, 1.0e-12)

for weights, empirical, interpolated in [
        ([1.0, 0.0], 0.0, 0.0),
        ([0.0, 1.0], 1.0, 1.0),
        ([1.0, 0.0, 1.0], 0.0, 0.0)]:
    zero_sample = ot.Sample([[float(i)] for i in range(len(weights))])
    zero_fixed = ot.FixedExperiment(zero_sample, ot.Point(weights))
    algo = otexp.QuantileSimulationAlgorithm(output, zero_fixed, 0.5, 0.95)
    algo.run()
    assert algo.getResult().getQuantileEstimate() == empirical
    algo.setEstimatorType(INTERPOLATED)
    algo.run()
    assert algo.getResult().getQuantileEstimate() == interpolated

# A constant deterministic sample has a zero-width Woodruff interval.
constant = ot.FixedExperiment(ot.Sample([[3.0]] * 5), ot.Point(5, 1.0))
algo = otexp.QuantileSimulationAlgorithm(output, constant, 0.5, 0.95)
algo.run()
constant_result = algo.getResult()
assert constant_result.getQuantileEstimate() == 3.0
assert constant_result.getStandardError() == 0.0
assert constant_result.getConfidenceInterval() == ot.Interval(3.0, 3.0)

# A degenerate i.i.d. output exercises the constant-sample exact fallback.
dirac = ot.Dirac(0.0)
dirac_output = ot.RandomVector(dirac)
algo = otexp.QuantileSimulationAlgorithm(
    dirac_output, ot.MonteCarloExperiment(dirac, 100), 0.5, 0.5)
algo.run()
dirac_result = algo.getResult()
assert dirac_result.getCoverageType() == COVERAGE_EXACT
assert dirac_result.getQuantileEstimate() == 0.0
assert dirac_result.getStandardError() == 0.0
assert dirac_result.getConfidenceInterval() == ot.Interval(0.0, 0.0)

# A degenerate randomized design exercises the zero-variance replicate path.
algo = otexp.QuantileSimulationAlgorithm(
    dirac_output, ot.LHSExperiment(dirac, 20, True, False), 0.5, 0.5)
algo.setReplicateCount(5)
algo.setKeepSample(True)
algo.run()
dirac_result = algo.getResult()
assert dirac_result.getCoverageType() == COVERAGE_ASYMPTOTIC
assert dirac_result.getQuantileEstimate() == 0.0
assert dirac_result.getStandardError() == 0.0
assert dirac_result.getConfidenceInterval() == ot.Interval(0.0, 0.0)
assert algo.getInputSample().getSize() == 20
assert algo.getWeights().getSize() == 20

# ---------------------------------------------------------------------
# 11. Required coverage combinations
# ---------------------------------------------------------------------
algo = otexp.QuantileSimulationAlgorithm(output, 0.5, 0.5)
algo.setSampleSize(100)
algo.setRequiredCoverage(COVERAGE_EXACT)
algo.run()
assert algo.getResult().getCoverageType() == COVERAGE_EXACT

# An exact interval satisfies an asymptotic requirement.
algo.setRequiredCoverage(COVERAGE_ASYMPTOTIC)
algo.run()
assert algo.getResult().getCoverageType() == COVERAGE_EXACT

# No implemented algorithm claims anytime-valid coverage.
algo.setRequiredCoverage(COVERAGE_ANYTIME_VALID)
with ott.assert_raises(TypeError):
    algo.run()

# A deterministic design cannot satisfy an asymptotic requirement.
algo = otexp.QuantileSimulationAlgorithm(output, fixed3, 0.5, 0.5)
algo.setRequiredCoverage(COVERAGE_ASYMPTOTIC)
with ott.assert_raises(TypeError):
    algo.run()
algo.setRequiredCoverage(COVERAGE_NONE)
algo.run()
assert algo.getResult().getCoverageType() == COVERAGE_NONE

# An experiment with the wrong dimension is rejected by the setter path too.
algo = otexp.QuantileSimulationAlgorithm(output, 0.5, 0.5)
algo.setExperiment(ot.LHSExperiment(ot.Normal(2), 100))
with ott.assert_raises(TypeError):
    algo.run()

# The public setters reject invalid values.
with ott.assert_raises(TypeError):
    algo.setOutput(ot.RandomVector(ot.RandomVectorImplementation()))
with ott.assert_raises(TypeError):
    algo.setAlpha(0.0)
with ott.assert_raises(TypeError):
    algo.setConfidenceLevel(1.0)
with ott.assert_raises(TypeError):
    algo.setEstimatorType(99)
with ott.assert_raises(TypeError):
    algo.setRequiredCoverage(99)
with ott.assert_raises(TypeError):
    algo.setSampleSize(0)
with ott.assert_raises(TypeError):
    algo.setReplicateCount(1)

print("OK")

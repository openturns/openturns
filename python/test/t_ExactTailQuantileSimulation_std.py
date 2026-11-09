#!/usr/bin/env python

import math
import os

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

EMPIRICAL_INVERSE = otexp.QuantileSimulationResultImplementation.EMPIRICAL_INVERSE
INTERPOLATED = otexp.QuantileSimulationResultImplementation.INTERPOLATED
COVERAGE_EXACT = otexp.QuantileSimulationResultImplementation.COVERAGE_EXACT
COVERAGE_ANYTIME_VALID = otexp.QuantileSimulationResultImplementation.COVERAGE_ANYTIME_VALID
NO_TAIL = otexp.QuantileSimulationResultImplementation.NO_TAIL
LOWER_TAIL = otexp.QuantileSimulationResultImplementation.LOWER_TAIL
UPPER_TAIL = otexp.QuantileSimulationResultImplementation.UPPER_TAIL

distribution = ot.Normal()
output = ot.RandomVector(distribution)


def sorted_values(sample):
    return sorted(sample[i, 0] for i in range(sample.getSize()))


def exact_diagnostics(size, alpha, beta):
    """Ranks, estimate index and adjusted bounds of the exact interval."""
    ranks = list(ot.QuantileConfidence(alpha, beta).computeBilateralRank(size))
    estimate_index = int(math.ceil(size * alpha)) - 1
    lower_rank = min(ranks[0], estimate_index)
    upper_rank = max(ranks[1], estimate_index)
    return ranks, estimate_index, lower_rank, upper_rank


# ---------------------------------------------------------------------
# 1. Exact equivalence with the full-sample estimator
# ---------------------------------------------------------------------
size = 500
alpha = 0.95
beta = 0.95
ot.RandomGenerator.SetSeed(20260201)
sample = distribution.getSample(size)
values = sorted_values(sample)
ranks, estimate_index, lower_rank, upper_rank = exact_diagnostics(size, alpha, beta)

ot.RandomGenerator.SetSeed(20260201)
algo = otexp.ExactTailQuantileSimulation(output, alpha, beta)
assert algo.getAlpha() == alpha
assert algo.getConfidenceLevel() == beta
assert algo.getEstimatorType() == EMPIRICAL_INVERSE
assert algo.getRequiredCoverage() == otexp.QuantileSimulationResultImplementation.COVERAGE_NONE
assert not algo.hasExperiment()
algo.setSampleSize(size)
assert algo.getSampleSize() == size
algo.run()
result = algo.getResult()

assert result.getQuantileEstimate() == values[estimate_index]
interval = result.getConfidenceInterval()
assert interval.getLowerBound()[0] == values[lower_rank]
assert interval.getUpperBound()[0] == values[upper_rank]
assert result.getCoverageType() == COVERAGE_EXACT
assert result.getCoverageDescription() == "Exact"
assert result.getEstimatorType() == EMPIRICAL_INVERSE
assert result.getSampleSize() == size
assert result.getEffectiveSampleSize() == float(size)
assert result.getBlockLength() == 0
assert result.getBlockCount() == 0
assert result.getTailType() == UPPER_TAIL
assert result.getTailDescription() == "Upper tail"
assert result.getLowerRank() == lower_rank
assert result.getUpperRank() == upper_rank
assert result.getRetainedCount() == size - lower_rank
assert result.getPeakMemory() == result.getRetainedCount()
assert result.getPeakMemoryInBytes() == 8 * result.getRetainedCount()
assert not result.hasBiasEstimate()
assert result.getBiasToStandardErrorRatio() == 0.0
assert result.isFixedSample()
assert result.getConvergenceHistory().getSize() == 1
assert result.drawQuantileConvergence().getDrawables().getSize() == 3
assert "ExactTailQuantileSimulation" in repr(algo)
assert "tail=" in repr(result)

# The standard error is the spacing estimate of the full-sample estimator.
full = otexp.MonteCarloQuantileSimulation(output, alpha, beta)
ot.RandomGenerator.SetSeed(20260201)
full.setSampleSize(size)
full.run()
full_result = full.getResult()
assert full_result.getQuantileEstimate() == result.getQuantileEstimate()
assert full_result.getConfidenceInterval() == result.getConfidenceInterval()
ott.assert_almost_equal(
    result.getStandardError(), full_result.getStandardError(), 1.0e-15, 1.0e-15)

# A lower confidence rank clamped to the estimate must not shorten the
# spacing window: the exact-tail standard error must still match the
# full-sample estimator.
size = 200
alpha = 0.99
beta = 0.2
ranks, estimate_index, lower_rank, upper_rank = exact_diagnostics(
    size, alpha, beta)
assert lower_rank == estimate_index
ot.RandomGenerator.SetSeed(20260205)
algo = otexp.ExactTailQuantileSimulation(output, alpha, beta)
algo.setSampleSize(size)
algo.run()
result = algo.getResult()
assert result.getTailType() == UPPER_TAIL
assert result.getRetainedCount() == size - (estimate_index - 1)
full = otexp.MonteCarloQuantileSimulation(output, alpha, beta)
full.setSampleSize(size)
ot.RandomGenerator.SetSeed(20260205)
full.run()
full_result = full.getResult()
assert result.getQuantileEstimate() == full_result.getQuantileEstimate()
assert result.getConfidenceInterval() == full_result.getConfidenceInterval()
ott.assert_almost_equal(
    result.getStandardError(), full_result.getStandardError(), 1.0e-15, 1.0e-15)

# ---------------------------------------------------------------------
# 2. Tail selection at extreme and central levels
# ---------------------------------------------------------------------
for level, expected_tail in [(0.01, LOWER_TAIL), (0.5, None),
                             (0.95, UPPER_TAIL), (0.99, UPPER_TAIL)]:
    ot.RandomGenerator.SetSeed(1000 + int(100 * level))
    algo = otexp.ExactTailQuantileSimulation(output, level, 0.95)
    algo.setSampleSize(1000)
    algo.run()
    result = algo.getResult()
    ranks, estimate_index, lower_rank, upper_rank = exact_diagnostics(1000, level, 0.95)

    lower_retained = upper_rank + 1
    upper_retained = 1000 - lower_rank
    retained = min(lower_retained, upper_retained)
    if expected_tail is None:
        # At the central level the binomial ranks are a near tie, so the
        # selected tail follows the last bit of the binomial CDF.  Derive
        # the expected tail from the same memory criterion as the estimator.
        expected_tail = (LOWER_TAIL if lower_retained <= upper_retained
                         else UPPER_TAIL)
    assert result.getTailType() == expected_tail, level
    assert result.getTailDescription() == ("Lower tail" if expected_tail == LOWER_TAIL else "Upper tail")
    assert result.getRetainedCount() == retained, level
    assert result.getLowerRank() == lower_rank
    assert result.getUpperRank() == upper_rank
    assert result.getConfidenceInterval().getLowerBound()[0] <= result.getQuantileEstimate()
    assert result.getQuantileEstimate() <= result.getConfidenceInterval().getUpperBound()[0]
    assert math.isfinite(result.getStandardError())
    assert result.getStandardError() >= 0.0

# At an extreme upper level the retained tail is a small fraction of N.
ot.RandomGenerator.SetSeed(20260202)
algo = otexp.ExactTailQuantileSimulation(output, 0.999, 0.95)
algo.setSampleSize(10000)
algo.run()
result = algo.getResult()
assert result.getTailType() == UPPER_TAIL
assert result.getRetainedCount() < 100
assert result.getPeakMemory() == result.getRetainedCount()

# The chunked stream is the same stream as a single full sample, so the
# retained order statistics are the full-sample order statistics.
ot.RandomGenerator.SetSeed(20260203)
sample = distribution.getSample(10000)
values = sorted_values(sample)
ranks, estimate_index, lower_rank, upper_rank = exact_diagnostics(10000, 0.999, 0.95)
ot.RandomGenerator.SetSeed(20260203)
algo = otexp.ExactTailQuantileSimulation(output, 0.999, 0.95)
algo.setSampleSize(10000)
algo.run()
result = algo.getResult()
assert result.getQuantileEstimate() == values[estimate_index]
assert result.getConfidenceInterval() == ot.Interval(values[lower_rank], values[upper_rank])

# A constant output exercises the degenerate spacing fallback.
constant_output = ot.RandomVector(ot.Dirac(1.0))
algo = otexp.ExactTailQuantileSimulation(constant_output, 0.95, 0.95)
algo.setSampleSize(500)
algo.run()
result = algo.getResult()
assert result.getQuantileEstimate() == 1.0
assert result.getStandardError() == 0.0
assert result.getConfidenceInterval() == ot.Interval(1.0, 1.0)

# ---------------------------------------------------------------------
# 3. Reproducibility
# ---------------------------------------------------------------------
ot.RandomGenerator.SetSeed(20260204)
algo1 = otexp.ExactTailQuantileSimulation(output, 0.95, 0.95)
algo1.setSampleSize(2000)
algo1.run()
first = algo1.getResult()
ot.RandomGenerator.SetSeed(20260204)
algo2 = otexp.ExactTailQuantileSimulation(output, 0.95, 0.95)
algo2.setSampleSize(2000)
algo2.run()
second = algo2.getResult()
assert first.getQuantileEstimate() == second.getQuantileEstimate()
assert first.getConfidenceInterval() == second.getConfidenceInterval()
assert first.getRetainedCount() == second.getRetainedCount()

# ---------------------------------------------------------------------
# 4. Experiment accessors and default experiment resolution
# ---------------------------------------------------------------------
mce = ot.MonteCarloExperiment(distribution, 5000)
algo = otexp.ExactTailQuantileSimulation(output, mce, 0.95, 0.95)
assert algo.hasExperiment()
assert algo.getSampleSize() == 5000
assert algo.getExperiment().getSize() == 5000
assert "MonteCarloExperiment" in repr(algo)
algo.setSampleSize(4000)
assert algo.getSampleSize() == 4000
assert algo.getExperiment().getSize() == 4000
algo.run()
assert algo.getResult().getSampleSize() == 4000

composite = ot.CompositeRandomVector(ot.SymbolicFunction(["x"], ["x^2"]), output)
algo = otexp.ExactTailQuantileSimulation(composite, 0.5, 0.5)
algo.setSampleSize(2000)
algo.run()
assert algo.getResult().getQuantileEstimate() >= 0.0

# The default constructor keeps the default levels.
default_algo = otexp.ExactTailQuantileSimulation()
assert default_algo.getAlpha() == 0.95
assert default_algo.getConfidenceLevel() == 0.95

# ---------------------------------------------------------------------
# 5. Negative limit tests
# ---------------------------------------------------------------------
algo = otexp.ExactTailQuantileSimulation(output, 0.95, 0.95)
with ott.assert_raises(TypeError):
    algo.setSampleSize(0)
with ott.assert_raises(TypeError):
    algo.setEstimatorType(99)
with ott.assert_raises(TypeError):
    algo.setRequiredCoverage(99)
with ott.assert_raises(TypeError):
    algo.run()

# The interpolated estimator has no exact order-statistic interval.
algo = otexp.ExactTailQuantileSimulation(output, 0.95, 0.95)
algo.setSampleSize(500)
algo.setEstimatorType(INTERPOLATED)
with ott.assert_raises(TypeError):
    algo.run()

# An exact interval cannot pretend to be anytime-valid.
algo = otexp.ExactTailQuantileSimulation(output, 0.95, 0.95)
algo.setSampleSize(500)
algo.setRequiredCoverage(COVERAGE_ANYTIME_VALID)
with ott.assert_raises(TypeError):
    algo.run()

# A sample too small for the requested exact levels is rejected.
algo = otexp.ExactTailQuantileSimulation(output, 0.999, 0.95)
algo.setSampleSize(10)
with ott.assert_raises(TypeError):
    algo.run()

# The estimator requires an independent experiment.
lhs = ot.LHSExperiment(distribution, 1000, True, True)
algo = otexp.ExactTailQuantileSimulation(output, lhs, 0.95, 0.95)
with ott.assert_raises(TypeError):
    algo.run()

# The estimator requires uniform weights.
importance = ot.ImportanceSamplingExperiment(
    distribution, ot.Normal(1.0, 1.0), 1000)
algo = otexp.ExactTailQuantileSimulation(output, importance, 0.95, 0.95)
with ott.assert_raises(TypeError):
    algo.run()

# A design with the wrong dimension is rejected.
algo = otexp.ExactTailQuantileSimulation(output, 0.95, 0.95)
algo.setSampleSize(1000)
algo.setExperiment(ot.MonteCarloExperiment(ot.Normal(2), 1000))
with ott.assert_raises(TypeError):
    algo.run()

# Invalid levels and multivariate outputs are rejected.
with ott.assert_raises(TypeError):
    otexp.ExactTailQuantileSimulation(output, 0.0, 0.95)
with ott.assert_raises(TypeError):
    otexp.ExactTailQuantileSimulation(output, 0.95, 1.0)
with ott.assert_raises(TypeError):
    otexp.ExactTailQuantileSimulation(ot.RandomVector(ot.Normal(2)), 0.95, 0.95)

# ---------------------------------------------------------------------
# 6. Persistence and handle
# ---------------------------------------------------------------------
ot.RandomGenerator.SetSeed(20260206)
algo = otexp.ExactTailQuantileSimulation(output, 0.95, 0.95)
algo.setSampleSize(4000)
algo.run()
result = algo.getResult()

path = "exact_tail_quantile_simulation.xml"
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager(path))
study.add("result", result)
study.add("algo", algo)
study.save()

study = ot.Study()
study.setStorageManager(ot.XMLStorageManager(path))
study.load()
loaded_algo = otexp.ExactTailQuantileSimulation()
study.fillObject("algo", loaded_algo)
loaded_result = otexp.QuantileSimulationResult()
study.fillObject("result", loaded_result)
os.remove(path)

assert loaded_algo.getSampleSize() == 4000
assert loaded_algo.getAlpha() == 0.95
assert loaded_algo.getConfidenceLevel() == 0.95
assert loaded_algo.getResult().getQuantileEstimate() == result.getQuantileEstimate()
assert loaded_result.getQuantileEstimate() == result.getQuantileEstimate()
assert loaded_result.getStandardError() == result.getStandardError()
assert loaded_result.getConfidenceInterval() == result.getConfidenceInterval()
assert loaded_result.getCoverageType() == COVERAGE_EXACT
assert loaded_result.getTailType() == result.getTailType()
assert loaded_result.getTailDescription() == result.getTailDescription()
assert loaded_result.getRetainedCount() == result.getRetainedCount()
assert loaded_result.getLowerRank() == result.getLowerRank()
assert loaded_result.getUpperRank() == result.getUpperRank()
assert loaded_result.getPeakMemory() == result.getPeakMemory()

handle = otexp.QuantileSimulation(algo)
assert isinstance(handle, otexp.QuantileSimulation)
assert handle.getSampleSize() == 4000
assert handle.getResult().getRetainedCount() == result.getRetainedCount()
ot.RandomGenerator.SetSeed(20260206)
handle.run()
assert handle.getResult().getQuantileEstimate() == result.getQuantileEstimate()
assert handle.getResult().getConfidenceInterval() == result.getConfidenceInterval()

# Default diagnostics of a result that was not produced by an exact-tail run.
empty_result = otexp.QuantileSimulationResult()
assert empty_result.getTailType() == NO_TAIL
assert empty_result.getTailDescription() == "None"
assert empty_result.getRetainedCount() == 0
assert empty_result.getLowerRank() == 0
assert empty_result.getUpperRank() == 0
assert empty_result.getPeakMemory() == 0

# Direct diagnostics accessors, including their validation.
empty_result.setTailType(LOWER_TAIL)
assert empty_result.getTailType() == LOWER_TAIL
assert empty_result.getTailDescription() == "Lower tail"
empty_result.setRetainedCount(7)
assert empty_result.getRetainedCount() == 7
empty_result.setLowerRank(2)
assert empty_result.getLowerRank() == 2
empty_result.setUpperRank(8)
assert empty_result.getUpperRank() == 8
with ott.assert_raises(TypeError):
    empty_result.setTailType(99)

# ---------------------------------------------------------------------
# 7. ResourceMap keys
# ---------------------------------------------------------------------
chunk_key = "ExactTailQuantileSimulation-MinimumChunkSize"
saved_chunk = ot.ResourceMap.GetAsUnsignedInteger(chunk_key)

# Chunking is an implementation detail: the same random stream must
# produce the same order statistics whatever the chunk size.
try:
    ot.ResourceMap.SetAsUnsignedInteger(chunk_key, 64)
    ot.RandomGenerator.SetSeed(20260207)
    algo = otexp.ExactTailQuantileSimulation(output, 0.999, 0.95)
    algo.setSampleSize(10000)
    algo.run()
    small_chunk_result = algo.getResult()

    ot.ResourceMap.SetAsUnsignedInteger(chunk_key, 8192)
    ot.RandomGenerator.SetSeed(20260207)
    algo = otexp.ExactTailQuantileSimulation(output, 0.999, 0.95)
    algo.setSampleSize(10000)
    algo.run()
    large_chunk_result = algo.getResult()

    assert small_chunk_result.getQuantileEstimate() == large_chunk_result.getQuantileEstimate()
    assert small_chunk_result.getConfidenceInterval() == large_chunk_result.getConfidenceInterval()
    assert small_chunk_result.getStandardError() == large_chunk_result.getStandardError()
    assert small_chunk_result.getRetainedCount() == large_chunk_result.getRetainedCount()
finally:
    ot.ResourceMap.SetAsUnsignedInteger(chunk_key, saved_chunk)

# A NaN output is rejected before it can corrupt the retained heaps.
nan_output = ot.CompositeRandomVector(
    ot.PythonFunction(1, 1, lambda x: [float("nan")]),
    ot.RandomVector(ot.Normal()),
)
algo = otexp.ExactTailQuantileSimulation(nan_output, 0.95, 0.95)
algo.setSampleSize(100)
with ott.assert_raises(TypeError):
    algo.run()

# A non-positive minimum chunk size is rejected.
try:
    ot.ResourceMap.SetAsUnsignedInteger(chunk_key, 0)
    algo = otexp.ExactTailQuantileSimulation(output, 0.95, 0.95)
    algo.setSampleSize(500)
    with ott.assert_raises(TypeError):
        algo.run()
finally:
    ot.ResourceMap.SetAsUnsignedInteger(chunk_key, saved_chunk)

print("OK")

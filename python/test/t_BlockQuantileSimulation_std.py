#!/usr/bin/env python

import math
import os

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

MEAN = otexp.QuantileSimulationResultImplementation.MEAN
MEDIAN = otexp.QuantileSimulationResultImplementation.MEDIAN
TRIMMED_MEAN = otexp.QuantileSimulationResultImplementation.TRIMMED_MEAN
COVERAGE_ASYMPTOTIC = otexp.QuantileSimulationResultImplementation.COVERAGE_ASYMPTOTIC
COVERAGE_EXACT = otexp.QuantileSimulationResultImplementation.COVERAGE_EXACT
COVERAGE_NONE = otexp.QuantileSimulationResultImplementation.COVERAGE_NONE

FIXED = otexp.BlockQuantileSimulation.FIXED
SQRT_N = otexp.BlockQuantileSimulation.SQRT_N
ASYMPTOTIC = otexp.BlockQuantileSimulation.ASYMPTOTIC
BIAS_TARGET = otexp.BlockQuantileSimulation.BIAS_TARGET
MEMORY_BOUND = otexp.BlockQuantileSimulation.MEMORY_BOUND
ADAPTIVE = otexp.BlockQuantileSimulation.ADAPTIVE

distribution = ot.Normal()
output = ot.RandomVector(distribution)


def stdev(values):
    mean = sum(values) / len(values)
    return math.sqrt(sum((value - mean) ** 2 for value in values) / (len(values) - 1))


def block_values(result):
    point = result.getBlockQuantileSample()
    return [point[i] for i in range(point.getSize())]


# ---------------------------------------------------------------------
# 1. Default asymptotic policy
# ---------------------------------------------------------------------
ot.RandomGenerator.SetSeed(20260101)
algo = otexp.BlockQuantileSimulation(output, 0.95, 0.95)
algo.setSampleSize(10000)
assert algo.getSampleSize() == 10000
assert not algo.hasExperiment()
assert algo.getBlockLengthPolicy() == ASYMPTOTIC
assert algo.getCombinationType() == MEAN
assert algo.getTrimmingFraction() == 0.1
assert algo.getMinimumBlockCount() == 30
assert algo.getBootstrapSize() == 0
algo.run()
result = algo.getResult()

expected_length = int(math.ceil(10000 ** (2.0 / 3.0)))
assert result.getBlockLength() == expected_length
assert result.getBlockCount() == 10000 // expected_length
assert result.getSampleSize() == result.getBlockLength() * result.getBlockCount()
assert result.getEffectiveSampleSize() == float(result.getSampleSize())
assert result.getBlockQuantileSample().getSize() == result.getBlockCount()
assert result.getCombinationType() == MEAN
assert result.getCombinationDescription() == "Mean"
assert result.getPeakMemory() == result.getBlockLength() + result.getBlockCount()
assert result.getPeakMemoryInBytes() == 8 * result.getPeakMemory()
assert result.getBlockLengthPolicyDescription().startswith("Asymptotic")
assert not result.hasBiasEstimate()
assert result.getBiasToStandardErrorRatio() == 0.0
assert result.getCoverageType() == COVERAGE_ASYMPTOTIC
assert result.getConvergenceHistory().getSize() == result.getBlockCount()
assert result.drawQuantileConvergence().getDrawables().getSize() == 3
assert result.isFixedSample()

values = block_values(result)
estimate = result.getQuantileEstimate()
ott.assert_almost_equal(estimate, sum(values) / len(values), 1.0e-12, 1.0e-12)
standard_error = stdev(values) / math.sqrt(len(values))
ott.assert_almost_equal(result.getStandardError(), standard_error, 1.0e-12, 1.0e-12)
t = ot.DistFunc.qStudent(len(values) - 1, 0.975)
interval = result.getConfidenceInterval()
ott.assert_almost_equal(interval.getLowerBound()[0], estimate - t * standard_error, 1.0e-12, 1.0e-12)
ott.assert_almost_equal(interval.getUpperBound()[0], estimate + t * standard_error, 1.0e-12, 1.0e-12)
assert abs(estimate - ot.Normal().computeQuantile(0.95)[0]) < 0.15
assert "BlockQuantileSimulation" in repr(algo)
assert "blockLength" in repr(result)

# ---------------------------------------------------------------------
# 2. Minimum-memory policy has no coverage guarantee
# ---------------------------------------------------------------------
ot.RandomGenerator.SetSeed(20260102)
algo = otexp.BlockQuantileSimulation(output, 0.95, 0.95)
algo.setSampleSize(10000)
algo.setBlockLengthPolicy(SQRT_N)
algo.run()
result = algo.getResult()
assert result.getBlockLength() == 100
assert result.getBlockCount() == 100
assert result.getCoverageType() == COVERAGE_NONE, "SQRT_N must not claim coverage"
assert result.getBlockLengthPolicyDescription().startswith("Square root")
assert result.getConfidenceInterval().getLowerBound()[0] <= result.getQuantileEstimate()
assert result.getQuantileEstimate() <= result.getConfidenceInterval().getUpperBound()[0]

# ---------------------------------------------------------------------
# 3. Fixed length, combinations and their standard errors
# ---------------------------------------------------------------------
ot.RandomGenerator.SetSeed(20260103)
algo = otexp.BlockQuantileSimulation(output, 0.95, 0.95)
algo.setSampleSize(6000)
algo.setBlockLength(200)
assert algo.getBlockLength() == 200
assert algo.getBlockLengthPolicy() == FIXED, "setBlockLength selects the FIXED policy"
algo.run()
result = algo.getResult()
assert result.getBlockLength() == 200
assert result.getBlockCount() == 30
assert result.getSampleSize() == 6000
assert result.getCoverageType() == COVERAGE_ASYMPTOTIC
assert result.getBlockLengthPolicyDescription().startswith("Fixed")
values = block_values(result)
sorted_values = sorted(values)

# Median combination and its efficiency-corrected standard error.
algo.setCombinationType(MEDIAN)
algo.run()
result = algo.getResult()
values = block_values(result)
sorted_values = sorted(values)
expected_median = 0.5 * (sorted_values[len(values) // 2 - 1] + sorted_values[len(values) // 2])
ott.assert_almost_equal(result.getQuantileEstimate(), expected_median, 1.0e-12, 1.0e-12)
expected_error = math.sqrt(0.5 * math.pi) * stdev(values) / math.sqrt(len(values))
ott.assert_almost_equal(result.getStandardError(), expected_error, 1.0e-12, 1.0e-12)
assert result.getCombinationType() == MEDIAN
assert result.getCombinationDescription() == "Median"

# Trimmed mean and its retained-count standard error.
algo.setCombinationType(TRIMMED_MEAN)
algo.setTrimmingFraction(0.2)
algo.run()
result = algo.getResult()
values = block_values(result)
sorted_values = sorted(values)
trimmed = int(math.floor(0.2 * len(values)))
retained = sorted_values[trimmed:len(values) - trimmed]
expected_mean = sum(retained) / len(retained)
ott.assert_almost_equal(result.getQuantileEstimate(), expected_mean, 1.0e-12, 1.0e-12)
ott.assert_almost_equal(
    result.getStandardError(), stdev(retained) / math.sqrt(len(retained)), 1.0e-12, 1.0e-12)
assert result.getCombinationType() == TRIMMED_MEAN
assert result.getTrimmingFraction() == 0.2
assert result.getCombinationDescription().startswith("Trimmed mean")

# ---------------------------------------------------------------------
# 4. Memory-bound policy
# ---------------------------------------------------------------------
ot.RandomGenerator.SetSeed(20260104)
algo = otexp.BlockQuantileSimulation(output, 0.95, 0.95)
algo.setSampleSize(100000)
algo.setMemoryBudget(5000)
algo.setBlockLengthPolicy(MEMORY_BOUND)
algo.run()
result = algo.getResult()
assert result.getBlockLength() + result.getBlockCount() <= 5000
assert result.getBlockCount() >= algo.getMinimumBlockCount(), "the block count floor is enforced"
assert result.getBlockLength() > math.sqrt(result.getSampleSize())
assert result.getCoverageType() == COVERAGE_ASYMPTOTIC
assert result.getBlockLengthPolicyDescription().startswith("Memory bound")

# A budget below 2*sqrt(N) cannot host a block estimator.
algo = otexp.BlockQuantileSimulation(output, 0.95, 0.95)
algo.setSampleSize(100000)
algo.setMemoryBudget(100)
algo.setBlockLengthPolicy(MEMORY_BOUND)
with ott.assert_raises(TypeError):
    algo.run()

# ---------------------------------------------------------------------
# 5. Pilot-based policies
# ---------------------------------------------------------------------
for policy, tolerance in [(BIAS_TARGET, 0.5), (ADAPTIVE, 0.2)]:
    ot.RandomGenerator.SetSeed(1000 + policy)
    algo = otexp.BlockQuantileSimulation(output, 0.95, 0.95)
    algo.setSampleSize(10000)
    algo.setBlockLengthPolicy(policy)
    algo.setBiasTolerance(tolerance)
    algo.run()
    result = algo.getResult()
    assert result.getBlockLength() >= math.ceil(math.sqrt(result.getSampleSize()))
    assert result.getBlockCount() >= 2
    assert result.hasBiasEstimate(), "a pilot must provide a bias estimate"
    assert math.isfinite(result.getBiasEstimate())
    assert result.getBiasToStandardErrorRatio() >= 0.0
    assert "Bias target" in result.getBlockLengthPolicyDescription() or \
        "Adaptive" in result.getBlockLengthPolicyDescription()

# ---------------------------------------------------------------------
# 6. Bootstrap interval
# ---------------------------------------------------------------------
ot.RandomGenerator.SetSeed(20260106)
algo = otexp.BlockQuantileSimulation(output, 0.95, 0.95)
algo.setSampleSize(6000)
algo.setBlockLength(200)
algo.setBootstrapSize(200)
algo.run()
result = algo.getResult()
assert algo.getBootstrapSize() == 200
assert result.getStandardError() >= 0.0
assert math.isfinite(result.getStandardError())
interval = result.getConfidenceInterval()
assert interval.getLowerBound()[0] <= result.getQuantileEstimate()
assert result.getQuantileEstimate() <= interval.getUpperBound()[0]

# ---------------------------------------------------------------------
# 7. Reproducibility
# ---------------------------------------------------------------------
ot.RandomGenerator.SetSeed(20260107)
algo1 = otexp.BlockQuantileSimulation(output, 0.95, 0.95)
algo1.setSampleSize(4000)
algo1.setBlockLength(100)
algo1.run()
first = algo1.getResult()
ot.RandomGenerator.SetSeed(20260107)
algo2 = otexp.BlockQuantileSimulation(output, 0.95, 0.95)
algo2.setSampleSize(4000)
algo2.setBlockLength(100)
algo2.run()
second = algo2.getResult()
assert first.getBlockQuantileSample() == second.getBlockQuantileSample()
assert first.getQuantileEstimate() == second.getQuantileEstimate()
assert first.getConfidenceInterval() == second.getConfidenceInterval()

# ---------------------------------------------------------------------
# 8. Experiment accessors and default experiment resolution
# ---------------------------------------------------------------------
mce = ot.MonteCarloExperiment(distribution, 5000)
algo = otexp.BlockQuantileSimulation(output, mce, 0.95, 0.95)
assert algo.hasExperiment()
assert algo.getSampleSize() == 5000
assert algo.getExperiment().getSize() == 5000
algo.setSampleSize(4000)
assert algo.getSampleSize() == 4000
assert algo.getExperiment().getSize() == 4000

composite = ot.CompositeRandomVector(
    ot.SymbolicFunction(["x"], ["x^2"]), output)
algo = otexp.BlockQuantileSimulation(composite, 0.5, 0.5)
algo.setSampleSize(2000)
algo.setBlockLengthPolicy(SQRT_N)
algo.run()
assert algo.getResult().getQuantileEstimate() >= 0.0

# A constant output exercises the degenerate variance branch.
constant_output = ot.RandomVector(ot.Dirac(1.0))
algo = otexp.BlockQuantileSimulation(constant_output, 0.95, 0.95)
algo.setSampleSize(500)
algo.setBlockLength(10)
algo.run()
result = algo.getResult()
assert result.getQuantileEstimate() == 1.0
assert result.getStandardError() == 0.0
assert result.getConfidenceInterval() == ot.Interval(1.0, 1.0)

# ---------------------------------------------------------------------
# 9. Negative limit tests
# ---------------------------------------------------------------------
algo = otexp.BlockQuantileSimulation(output, 0.95, 0.95)
with ott.assert_raises(TypeError):
    algo.setBlockLength(0)
with ott.assert_raises(TypeError):
    algo.setBlockLengthPolicy(99)
with ott.assert_raises(TypeError):
    algo.setCombinationType(99)
with ott.assert_raises(TypeError):
    algo.setTrimmingFraction(-0.1)
with ott.assert_raises(TypeError):
    algo.setTrimmingFraction(0.5)
with ott.assert_raises(TypeError):
    algo.setMinimumBlockCount(1)
with ott.assert_raises(TypeError):
    algo.setMemoryBudget(0)
with ott.assert_raises(TypeError):
    algo.setBiasTolerance(0.0)
with ott.assert_raises(TypeError):
    algo.setBiasTolerance(float("inf"))
with ott.assert_raises(TypeError):
    algo.setBootstrapSize(1)
with ott.assert_raises(TypeError):
    algo.setSampleSize(0)
with ott.assert_raises(TypeError):
    algo.run()

# A block length that leaves a single block is rejected.
algo = otexp.BlockQuantileSimulation(output, 0.95, 0.95)
algo.setSampleSize(1000)
algo.setBlockLength(600)
with ott.assert_raises(TypeError):
    algo.run()

# The FIXED policy requires an explicit length and MEMORY_BOUND a budget.
algo = otexp.BlockQuantileSimulation(output, 0.95, 0.95)
algo.setSampleSize(1000)
algo.setBlockLengthPolicy(FIXED)
with ott.assert_raises(TypeError):
    algo.run()
algo = otexp.BlockQuantileSimulation(output, 0.95, 0.95)
algo.setSampleSize(1000)
algo.setBlockLengthPolicy(MEMORY_BOUND)
with ott.assert_raises(TypeError):
    algo.run()

# The block estimator requires an independent experiment.
lhs = ot.LHSExperiment(distribution, 1000, True, True)
algo = otexp.BlockQuantileSimulation(output, lhs, 0.95, 0.95)
with ott.assert_raises(TypeError):
    algo.run()

# The block estimator requires uniform weights.
importance = ot.ImportanceSamplingExperiment(
    distribution, ot.Normal(1.0, 1.0), 1000)
algo = otexp.BlockQuantileSimulation(output, importance, 0.95, 0.95)
with ott.assert_raises(TypeError):
    algo.run()

# A design with the wrong dimension is rejected.
algo = otexp.BlockQuantileSimulation(output, 0.95, 0.95)
algo.setSampleSize(1000)
algo.setExperiment(ot.MonteCarloExperiment(ot.Normal(2), 1000))
with ott.assert_raises(TypeError):
    algo.run()

# Exact coverage cannot be claimed by the block estimator.
algo = otexp.BlockQuantileSimulation(output, 0.95, 0.95)
algo.setSampleSize(6000)
algo.setBlockLength(200)
algo.setRequiredCoverage(COVERAGE_EXACT)
with ott.assert_raises(TypeError):
    algo.run()

# A multivariate output is rejected at construction time.
with ott.assert_raises(TypeError):
    otexp.BlockQuantileSimulation(ot.RandomVector(ot.Normal(2)), 0.95, 0.95)

# ---------------------------------------------------------------------
# 10. Persistence and handle
# ---------------------------------------------------------------------
ot.RandomGenerator.SetSeed(20260110)
algo = otexp.BlockQuantileSimulation(output, 0.95, 0.95)
algo.setSampleSize(6000)
algo.setBlockLength(200)
algo.setCombinationType(TRIMMED_MEAN)
algo.setTrimmingFraction(0.1)
algo.setMinimumBlockCount(20)
algo.setMemoryBudget(4000)
algo.setBiasTolerance(0.25)
algo.setBootstrapSize(50)
algo.run()
result = algo.getResult()

path = "block_quantile_simulation.xml"
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager(path))
study.add("result", result)
study.add("algo", algo)
study.save()

study = ot.Study()
study.setStorageManager(ot.XMLStorageManager(path))
study.load()
loaded_algo = otexp.BlockQuantileSimulation()
study.fillObject("algo", loaded_algo)
loaded_result = otexp.QuantileSimulationResult()
study.fillObject("result", loaded_result)
os.remove(path)

assert loaded_algo.getBlockLengthPolicy() == FIXED
assert loaded_algo.getBlockLength() == 200
assert loaded_algo.getCombinationType() == TRIMMED_MEAN
assert loaded_algo.getTrimmingFraction() == 0.1
assert loaded_algo.getMinimumBlockCount() == 20
assert loaded_algo.getMemoryBudget() == 4000
assert loaded_algo.getBiasTolerance() == 0.25
assert loaded_algo.getBootstrapSize() == 50
assert loaded_result.getQuantileEstimate() == result.getQuantileEstimate()
assert loaded_result.getConfidenceInterval() == result.getConfidenceInterval()
assert loaded_result.getBlockLength() == result.getBlockLength()
assert loaded_result.getBlockCount() == result.getBlockCount()
assert loaded_result.getBlockQuantileSample() == result.getBlockQuantileSample()
assert loaded_result.getCombinationType() == TRIMMED_MEAN
assert loaded_result.getTrimmingFraction() == 0.1
assert loaded_result.getPeakMemory() == result.getPeakMemory()
assert loaded_result.getBlockLengthPolicyDescription() == result.getBlockLengthPolicyDescription()

handle = otexp.QuantileSimulation(algo)
assert isinstance(handle, otexp.QuantileSimulation)
assert handle.getSampleSize() == 6000
assert handle.getResult().getBlockLength() == result.getBlockLength()
ot.RandomGenerator.SetSeed(20260110)
handle.run()
assert handle.getResult().getBlockLength() == result.getBlockLength()
assert handle.getResult().getBlockCount() == result.getBlockCount()

# ---------------------------------------------------------------------
# 11. Result diagnostics accessors
# ---------------------------------------------------------------------
diagnostic = otexp.QuantileSimulationResult()
diagnostic.setBlockCount(5)
assert diagnostic.getBlockCount() == 5
diagnostic.setBlockQuantileSample([1.0, 2.0, 3.0])
assert diagnostic.getBlockQuantileSample() == ot.Point([1.0, 2.0, 3.0])
diagnostic.setCombinationType(MEDIAN)
assert diagnostic.getCombinationType() == MEDIAN
assert diagnostic.getCombinationDescription() == "Median"
diagnostic.setTrimmingFraction(0.25)
assert diagnostic.getTrimmingFraction() == 0.25
diagnostic.setBlockLengthPolicyDescription("manual policy")
assert diagnostic.getBlockLengthPolicyDescription() == "manual policy"
diagnostic.setPeakMemory(12)
assert diagnostic.getPeakMemory() == 12
assert diagnostic.getPeakMemoryInBytes() == 8 * 12
diagnostic.setBiasEstimate(-0.5)
diagnostic.setStandardError(1.0)
assert diagnostic.getBiasToStandardErrorRatio() == 0.5
with ott.assert_raises(TypeError):
    diagnostic.setBlockQuantileSample([1.0, float("nan")])
with ott.assert_raises(TypeError):
    diagnostic.setCombinationType(99)
with ott.assert_raises(TypeError):
    diagnostic.setTrimmingFraction(0.5)

# ---------------------------------------------------------------------
# 12. ResourceMap defaults and validation
# ---------------------------------------------------------------------
policy_key = "BlockQuantileSimulation-DefaultBlockLengthPolicy"
bias_key = "BlockQuantileSimulation-DefaultBiasTolerance"
adaptive_key = "BlockQuantileSimulation-DefaultAdaptiveTolerance"
bootstrap_key = "BlockQuantileSimulation-DefaultBootstrapSize"
minimum_pilot_key = "BlockQuantileSimulation-PilotMinimumSize"
maximum_pilot_key = "BlockQuantileSimulation-PilotMaximumSize"
pilot_fraction_key = "BlockQuantileSimulation-PilotSizeFraction"
block_count_key = "QuantileSimulation-MinimumBlockCount"

saved_policy = ot.ResourceMap.GetAsString(policy_key)
try:
    ot.ResourceMap.SetAsString(policy_key, "SQRT_N")
    algo = otexp.BlockQuantileSimulation(output, 0.95, 0.95)
    assert algo.getBlockLengthPolicy() == SQRT_N
    ot.ResourceMap.SetAsString(policy_key, "ADAPTIVE")
    algo = otexp.BlockQuantileSimulation(output, 0.95, 0.95)
    assert algo.getBlockLengthPolicy() == ADAPTIVE
finally:
    ot.ResourceMap.SetAsString(policy_key, saved_policy)
# The enumerated key only accepts the documented policy names.
with ott.assert_raises(TypeError):
    ot.ResourceMap.SetAsString(policy_key, "UNKNOWN")
assert ot.ResourceMap.GetAsString(policy_key) == saved_policy

saved_bias = ot.ResourceMap.GetAsScalar(bias_key)
try:
    ot.ResourceMap.SetAsScalar(bias_key, 0.25)
    algo = otexp.BlockQuantileSimulation(output, 0.95, 0.95)
    assert algo.getBiasTolerance() == 0.25
finally:
    ot.ResourceMap.SetAsScalar(bias_key, saved_bias)
try:
    ot.ResourceMap.SetAsScalar(bias_key, 0.0)
    algo = otexp.BlockQuantileSimulation(output, 0.95, 0.95)
    algo.setSampleSize(1000)
    algo.setBlockLength(10)
    with ott.assert_raises(TypeError):
        algo.run()
finally:
    ot.ResourceMap.SetAsScalar(bias_key, saved_bias)

saved_adaptive = ot.ResourceMap.GetAsScalar(adaptive_key)
try:
    ot.ResourceMap.SetAsScalar(adaptive_key, 0.05)
    ot.RandomGenerator.SetSeed(20260112)
    algo = otexp.BlockQuantileSimulation(output, 0.95, 0.95)
    algo.setSampleSize(4000)
    algo.setBlockLengthPolicy(ADAPTIVE)
    algo.run()
    description = algo.getResult().getBlockLengthPolicyDescription()
    assert description.startswith("Adaptive")
    assert "tolerance=0.05" in description
finally:
    ot.ResourceMap.SetAsScalar(adaptive_key, saved_adaptive)
try:
    ot.ResourceMap.SetAsScalar(adaptive_key, 0.0)
    algo = otexp.BlockQuantileSimulation(output, 0.95, 0.95)
    algo.setSampleSize(4000)
    algo.setBlockLengthPolicy(ADAPTIVE)
    with ott.assert_raises(TypeError):
        algo.run()
finally:
    ot.ResourceMap.SetAsScalar(adaptive_key, saved_adaptive)

saved_bootstrap = ot.ResourceMap.GetAsUnsignedInteger(bootstrap_key)
try:
    ot.ResourceMap.SetAsUnsignedInteger(bootstrap_key, 20)
    ot.RandomGenerator.SetSeed(20260113)
    algo = otexp.BlockQuantileSimulation(output, 0.95, 0.95)
    algo.setSampleSize(4000)
    algo.setBlockLength(100)
    assert algo.getBootstrapSize() == 20
    algo.run()
    assert math.isfinite(algo.getResult().getStandardError())
finally:
    ot.ResourceMap.SetAsUnsignedInteger(bootstrap_key, saved_bootstrap)
try:
    ot.ResourceMap.SetAsUnsignedInteger(bootstrap_key, 1)
    algo = otexp.BlockQuantileSimulation(output, 0.95, 0.95)
    algo.setSampleSize(1000)
    algo.setBlockLength(10)
    with ott.assert_raises(TypeError):
        algo.run()
finally:
    ot.ResourceMap.SetAsUnsignedInteger(bootstrap_key, saved_bootstrap)

saved_minimum_pilot = ot.ResourceMap.GetAsUnsignedInteger(minimum_pilot_key)
saved_maximum_pilot = ot.ResourceMap.GetAsUnsignedInteger(maximum_pilot_key)
saved_pilot_fraction = ot.ResourceMap.GetAsScalar(pilot_fraction_key)


def run_bias_target():
    ot.RandomGenerator.SetSeed(20260114)
    algo = otexp.BlockQuantileSimulation(output, 0.95, 0.95)
    algo.setSampleSize(5000)
    algo.setBlockLengthPolicy(BIAS_TARGET)
    algo.setBiasTolerance(0.25)
    algo.run()
    return algo.getResult()


try:
    ot.ResourceMap.SetAsUnsignedInteger(minimum_pilot_key, 500)
    assert run_bias_target().hasBiasEstimate()
finally:
    ot.ResourceMap.SetAsUnsignedInteger(minimum_pilot_key, saved_minimum_pilot)
try:
    ot.ResourceMap.SetAsUnsignedInteger(minimum_pilot_key, 500)
    ot.ResourceMap.SetAsUnsignedInteger(maximum_pilot_key, 2000)
    assert run_bias_target().hasBiasEstimate()
finally:
    ot.ResourceMap.SetAsUnsignedInteger(minimum_pilot_key, saved_minimum_pilot)
    ot.ResourceMap.SetAsUnsignedInteger(maximum_pilot_key, saved_maximum_pilot)
try:
    ot.ResourceMap.SetAsScalar(pilot_fraction_key, 0.2)
    assert run_bias_target().hasBiasEstimate()
finally:
    ot.ResourceMap.SetAsScalar(pilot_fraction_key, saved_pilot_fraction)

try:
    ot.ResourceMap.SetAsUnsignedInteger(minimum_pilot_key, 0)
    with ott.assert_raises(TypeError):
        run_bias_target()
finally:
    ot.ResourceMap.SetAsUnsignedInteger(minimum_pilot_key, saved_minimum_pilot)
try:
    ot.ResourceMap.SetAsUnsignedInteger(minimum_pilot_key, 1000)
    ot.ResourceMap.SetAsUnsignedInteger(maximum_pilot_key, 10)
    with ott.assert_raises(TypeError):
        run_bias_target()
finally:
    ot.ResourceMap.SetAsUnsignedInteger(minimum_pilot_key, saved_minimum_pilot)
    ot.ResourceMap.SetAsUnsignedInteger(maximum_pilot_key, saved_maximum_pilot)
try:
    ot.ResourceMap.SetAsScalar(pilot_fraction_key, 1.5)
    with ott.assert_raises(TypeError):
        run_bias_target()
finally:
    ot.ResourceMap.SetAsScalar(pilot_fraction_key, saved_pilot_fraction)

saved_block_count = ot.ResourceMap.GetAsUnsignedInteger(block_count_key)
try:
    ot.ResourceMap.SetAsUnsignedInteger(block_count_key, 5)
    algo = otexp.BlockQuantileSimulation(output, 0.95, 0.95)
    assert algo.getMinimumBlockCount() == 5
    algo.setSampleSize(6000)
    algo.setBlockLength(200)
    algo.run()
    assert algo.getResult().getBlockCount() >= 5
finally:
    ot.ResourceMap.SetAsUnsignedInteger(block_count_key, saved_block_count)
try:
    ot.ResourceMap.SetAsUnsignedInteger(block_count_key, 1)
    algo = otexp.BlockQuantileSimulation(output, 0.95, 0.95)
    algo.setSampleSize(1000)
    algo.setBlockLength(10)
    with ott.assert_raises(TypeError):
        algo.run()
finally:
    ot.ResourceMap.SetAsUnsignedInteger(block_count_key, saved_block_count)

print("OK")

#!/usr/bin/env python

import math
import os

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

EMPIRICAL_INVERSE = otexp.QuantileSimulationResultImplementation.EMPIRICAL_INVERSE
INTERPOLATED = otexp.QuantileSimulationResultImplementation.INTERPOLATED
COVERAGE_ASYMPTOTIC = otexp.QuantileSimulationResultImplementation.COVERAGE_ASYMPTOTIC
LOWER_TAIL = otexp.QuantileSimulationResultImplementation.LOWER_TAIL
UPPER_TAIL = otexp.QuantileSimulationResultImplementation.UPPER_TAIL

distribution = ot.Normal()
output = ot.RandomVector(distribution)
true_upper = distribution.computeQuantile(0.95)[0]
true_lower = distribution.computeQuantile(0.05)[0]


def point_values(point):
    return [point[i] for i in range(point.getSize())]


def mean(values):
    return sum(values) / len(values)


def stdev(values):
    center = mean(values)
    return math.sqrt(sum((value - center) ** 2 for value in values) / (len(values) - 1))


# ---------------------------------------------------------------------
# 1. Defaults and accessors
# ---------------------------------------------------------------------
algo = otexp.SubsetSamplingQuantile(output)
assert algo.getAlpha() == 0.95
assert algo.getConfidenceLevel() == 0.95
assert algo.getConditionalProbability() == 0.1
assert algo.getProposalRange() == 2.0
assert algo.getBurnIn() == 10
assert algo.getReplicateCount() == 30
assert algo.getParticleCount() == 1000
assert algo.getSampleSize() == 1000
assert algo.getEstimatorType() == EMPIRICAL_INVERSE

algo.setConditionalProbability(0.2)
algo.setProposalRange(1.5)
algo.setBurnIn(5)
algo.setReplicateCount(12)
algo.setParticleCount(400)
assert algo.getConditionalProbability() == 0.2
assert algo.getProposalRange() == 1.5
assert algo.getBurnIn() == 5
assert algo.getReplicateCount() == 12
assert algo.getParticleCount() == 400
assert algo.getSampleSize() == 400
# setSampleSize and setParticleCount are aliases
algo.setSampleSize(600)
assert algo.getParticleCount() == 600
assert algo.getSampleSize() == 600

# ---------------------------------------------------------------------
# 2. Upper-tail estimate and diagnostics
# ---------------------------------------------------------------------
particle_count = 1000
replicate_count = 20
conditional_probability = 0.1
ot.RandomGenerator.SetSeed(20260301)
algo = otexp.SubsetSamplingQuantile(output, 0.95, 0.95)
algo.setSampleSize(particle_count)
algo.setReplicateCount(replicate_count)
algo.setConditionalProbability(conditional_probability)
algo.run()
result = algo.getResult()

estimate = result.getQuantileEstimate()
assert abs(estimate - true_upper) < 0.1
assert result.getStandardError() > 0.0
interval = result.getConfidenceInterval()
assert interval.getLowerBound()[0] <= true_upper <= interval.getUpperBound()[0]
assert result.getEstimatorType() == EMPIRICAL_INVERSE
assert result.getCoverageType() == COVERAGE_ASYMPTOTIC
assert result.getTailType() == UPPER_TAIL
assert result.getTailDescription() == "Upper tail"
assert result.getParticleCount() == particle_count
assert result.getConditionalProbability() == conditional_probability
assert result.getReplicateCount() == replicate_count
assert result.getEffectiveSampleSize() == float(replicate_count)
assert result.getSampleSize() >= replicate_count * particle_count
assert not result.isFixedSample()

# The estimate is the mean of the replicate estimates and the interval
# is the Student interval built from their sample standard deviation.
replicates = point_values(result.getReplicateEstimates())
assert len(replicates) == replicate_count
ott.assert_almost_equal(estimate, mean(replicates), 1.0e-12, 1.0e-12)
standard_error = stdev(replicates) / math.sqrt(replicate_count)
ott.assert_almost_equal(result.getStandardError(), standard_error, 1.0e-12, 1.0e-12)
t = ot.DistFunc.qStudent(replicate_count - 1, 0.975)
ott.assert_almost_equal(
    interval.getLowerBound()[0], estimate - t * standard_error, 1.0e-12, 1.0e-12
)
ott.assert_almost_equal(
    interval.getUpperBound()[0], estimate + t * standard_error, 1.0e-12, 1.0e-12
)

# The number of levels is m = ceil(log(p) / log(p0)); only the m - 1
# splitting thresholds are recorded, with level probabilities p0^j.
tail_probability = 0.05
level_count = int(math.ceil(math.log(tail_probability) / math.log(conditional_probability)))
conditional_level_count = level_count - 1
assert result.getStepCount() == conditional_level_count
thresholds = result.getThresholdSequence()
levels = point_values(result.getLevelProbabilitySequence())
assert len(thresholds) == conditional_level_count
assert len(levels) == conditional_level_count
for j, level in enumerate(levels):
    ott.assert_almost_equal(level, conditional_probability ** (j + 1), 1.0e-12, 1.0e-12)
for value in point_values(thresholds):
    assert math.isfinite(value)
assert 0.0 < result.getAcceptanceRate() < 1.0

# A conditional probability that is not a multiple of 1/N is
# discretized to the realizable fraction floor(p0*N)/N.  Here p0*N =
# 1.5, so one particle out of ten is retained at the single level.
ot.RandomGenerator.SetSeed(20260304)
algo = otexp.SubsetSamplingQuantile(output, 0.95, 0.95)
algo.setSampleSize(10)
algo.setConditionalProbability(0.15)
algo.setReplicateCount(3)
algo.run()
discrete_result = algo.getResult()
discrete_levels = point_values(discrete_result.getLevelProbabilitySequence())
assert len(discrete_levels) == 1
ott.assert_almost_equal(discrete_levels[0], 0.1, 1.0e-12, 1.0e-12)

# ---------------------------------------------------------------------
# 3. Lower-tail symmetry
# ---------------------------------------------------------------------
ot.RandomGenerator.SetSeed(20260302)
algo = otexp.SubsetSamplingQuantile(output, 0.05, 0.95)
algo.setSampleSize(particle_count)
algo.setReplicateCount(replicate_count)
algo.run()
lower_result = algo.getResult()
assert abs(lower_result.getQuantileEstimate() - true_lower) < 0.1
lower_interval = lower_result.getConfidenceInterval()
assert lower_interval.getLowerBound()[0] <= true_lower <= lower_interval.getUpperBound()[0]
assert lower_result.getTailType() == LOWER_TAIL
assert lower_result.getTailDescription() == "Lower tail"
for j, level in enumerate(point_values(lower_result.getLevelProbabilitySequence())):
    ott.assert_almost_equal(level, conditional_probability ** (j + 1), 1.0e-12, 1.0e-12)

# ---------------------------------------------------------------------
# 4. Multidimensional input through a composite output
# ---------------------------------------------------------------------
input_distribution = ot.Normal(5)
input_vector = ot.RandomVector(input_distribution)
sum_function = ot.SymbolicFunction(
    [f"x{i}" for i in range(5)], ["x0 + x1 + x2 + x3 + x4"]
)
composite_output = ot.CompositeRandomVector(sum_function, input_vector)
true_sum_quantile = ot.Normal().computeQuantile(0.95)[0] * math.sqrt(5.0)
ot.RandomGenerator.SetSeed(20260303)
algo = otexp.SubsetSamplingQuantile(composite_output, 0.95, 0.95)
algo.setSampleSize(700)
algo.setReplicateCount(10)
algo.run()
result = algo.getResult()
assert abs(result.getQuantileEstimate() - true_sum_quantile) < 0.2
assert result.getConfidenceInterval().getLowerBound()[0] <= true_sum_quantile
assert true_sum_quantile <= result.getConfidenceInterval().getUpperBound()[0]

# ---------------------------------------------------------------------
# 5. Persistence and copy
# ---------------------------------------------------------------------
ot.RandomGenerator.SetSeed(20260304)
algo = otexp.SubsetSamplingQuantile(output, 0.95, 0.95)
algo.setSampleSize(600)
algo.setReplicateCount(12)
algo.setConditionalProbability(0.2)
algo.setProposalRange(1.5)
algo.setBurnIn(5)
algo.run()
result = algo.getResult()

path = "subset_sampling_quantile.xml"
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager(path))
study.add("result", result)
study.add("algo", algo)
study.save()

study = ot.Study()
study.setStorageManager(ot.XMLStorageManager(path))
study.load()
loaded_algo = otexp.SubsetSamplingQuantile()
study.fillObject("algo", loaded_algo)
loaded_result = otexp.QuantileSimulationResult()
study.fillObject("result", loaded_result)
os.remove(path)

assert loaded_algo.getSampleSize() == 600
assert loaded_algo.getReplicateCount() == 12
assert loaded_algo.getConditionalProbability() == 0.2
assert loaded_algo.getProposalRange() == 1.5
assert loaded_algo.getBurnIn() == 5
assert loaded_algo.getResult().getQuantileEstimate() == result.getQuantileEstimate()
assert loaded_result.getQuantileEstimate() == result.getQuantileEstimate()
assert loaded_result.getStandardError() == result.getStandardError()
assert loaded_result.getConfidenceInterval() == result.getConfidenceInterval()
assert loaded_result.getCoverageType() == COVERAGE_ASYMPTOTIC
assert loaded_result.getTailType() == UPPER_TAIL
assert loaded_result.getStepCount() == result.getStepCount()
assert loaded_result.getThresholdSequence() == result.getThresholdSequence()
assert loaded_result.getLevelProbabilitySequence() == result.getLevelProbabilitySequence()
assert loaded_result.getAcceptanceRate() == result.getAcceptanceRate()
assert loaded_result.getParticleCount() == result.getParticleCount()
assert loaded_result.getReplicateCount() == result.getReplicateCount()
assert loaded_result.getReplicateEstimates() == result.getReplicateEstimates()
assert loaded_result.getConditionalProbability() == result.getConditionalProbability()

copied = otexp.SubsetSamplingQuantile(algo)
assert copied.getSampleSize() == 600
assert copied.getReplicateCount() == 12
assert copied.getConditionalProbability() == 0.2

# ---------------------------------------------------------------------
# 6. Invalid arguments
# ---------------------------------------------------------------------
with ott.assert_raises(TypeError):
    otexp.SubsetSamplingQuantile(output, 0.0, 0.95)
with ott.assert_raises(TypeError):
    otexp.SubsetSamplingQuantile(output, 1.0, 0.95)
with ott.assert_raises(TypeError):
    otexp.SubsetSamplingQuantile(output, 0.95, 0.0)
with ott.assert_raises(TypeError):
    otexp.SubsetSamplingQuantile(output, 0.95, 1.0)
with ott.assert_raises(TypeError):
    otexp.SubsetSamplingQuantile(ot.RandomVector(ot.Normal(2)), 0.95, 0.95)

algo = otexp.SubsetSamplingQuantile(output)
with ott.assert_raises(TypeError):
    algo.setConditionalProbability(0.0)
with ott.assert_raises(TypeError):
    algo.setConditionalProbability(1.0)
with ott.assert_raises(TypeError):
    algo.setProposalRange(0.0)
with ott.assert_raises(TypeError):
    algo.setProposalRange(-1.0)
with ott.assert_raises(TypeError):
    algo.setProposalRange(float("inf"))
with ott.assert_raises(TypeError):
    algo.setBurnIn(0)
with ott.assert_raises(TypeError):
    algo.setReplicateCount(1)
with ott.assert_raises(TypeError):
    algo.setSampleSize(0)

# The replicated estimator is an empirical inverse: reject the
# interpolated convention before running the level loop.
algo = otexp.SubsetSamplingQuantile(output)
algo.setEstimatorType(INTERPOLATED)
with ott.assert_raises(TypeError):
    algo.run()

# A particle count smaller than 1 / p0 cannot retain a particle at a level.
algo = otexp.SubsetSamplingQuantile(output)
algo.setSampleSize(5)
algo.setConditionalProbability(0.1)
with ott.assert_raises(TypeError):
    algo.run()

print("OK")

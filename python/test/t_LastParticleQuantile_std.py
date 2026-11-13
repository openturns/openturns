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


# ---------------------------------------------------------------------
# 1. Defaults and accessors
# ---------------------------------------------------------------------
algo = otexp.LastParticleQuantile(output)
assert algo.getAlpha() == 0.95
assert algo.getConfidenceLevel() == 0.95
assert algo.getParticleCount() == 1000
assert algo.getSampleSize() == 1000
assert algo.getBurnIn() == 10
assert algo.getProposalRange() == 2.0
assert algo.getEstimatorType() == EMPIRICAL_INVERSE

algo.setParticleCount(400)
assert algo.getParticleCount() == 400
assert algo.getSampleSize() == 400
# setSampleSize and setParticleCount are aliases
algo.setSampleSize(600)
assert algo.getParticleCount() == 600
assert algo.getSampleSize() == 600
algo.setBurnIn(5)
algo.setProposalRange(1.5)
assert algo.getBurnIn() == 5
assert algo.getProposalRange() == 1.5

# ---------------------------------------------------------------------
# 2. Upper-tail estimate, Poisson ranks and diagnostics
# ---------------------------------------------------------------------
particle_count = 1000
tail_probability = 0.05
ot.RandomGenerator.SetSeed(20260309)
algo = otexp.LastParticleQuantile(output, 0.95, 0.95)
algo.setParticleCount(particle_count)
algo.run()
result = algo.getResult()

estimate = result.getQuantileEstimate()
assert abs(estimate - true_upper) < 0.1
assert result.getStandardError() > 0.0
interval = result.getConfidenceInterval()
assert interval.getLowerBound()[0] <= true_upper <= interval.getUpperBound()[0]
assert interval.getLowerBound()[0] <= estimate <= interval.getUpperBound()[0]
assert result.getEstimatorType() == EMPIRICAL_INVERSE
assert result.getCoverageType() == COVERAGE_ASYMPTOTIC
assert result.getTailType() == UPPER_TAIL
assert result.getTailDescription() == "Upper tail"
assert result.getParticleCount() == particle_count
assert result.getEffectiveSampleSize() == float(particle_count)
assert result.getSampleSize() > particle_count
assert not result.isFixedSample()
assert 0.0 < result.getAcceptanceRate() <= 1.0

# The threshold sequence has one value per move and is nondecreasing for
# the upper tail.  The Poisson interval [t_L, t_U] covers the quantile
# when the first crossing index K satisfies L < K <= U, so its exact
# Poisson coverage is at least the requested confidence level.
thresholds = point_values(result.getThresholdSequence())
lower_rank = result.getPoissonRankLower()
upper_rank = result.getPoissonRankUpper()
assert result.hasPoissonRanks()
assert 1 <= lower_rank <= upper_rank
assert result.getStepCount() == len(thresholds)
assert result.getStepCount() >= upper_rank
for previous, current in zip(thresholds, thresholds[1:]):
    assert current >= previous
crossing = next(
    index + 1 for index, value in enumerate(thresholds) if value >= true_upper
)
assert lower_rank < crossing <= upper_rank
poisson = ot.Poisson(-particle_count * math.log(tail_probability))
assert (
    poisson.computeCDF(upper_rank - 1) - poisson.computeCDF(lower_rank - 1)
    >= algo.getConfidenceLevel() - 1.0e-12
)

# Counterexample for the old rank convention: with N=10, p=exp(-2) and
# beta=0.99, shifting the lower rank down and leaving the upper rank
# unshifted gives F(U-1)-F(L-2) < beta.  The corrected convention shifts
# the upper rank up instead, which restores the nominal coverage.
lambda_moves = 20.0
poisson = ot.Poisson(lambda_moves)
lower = int(ot.DistFunc.qPoisson(lambda_moves, 0.005))
upper = int(ot.DistFunc.qPoisson(lambda_moves, 0.995))
assert poisson.computeCDF(upper - 1) - poisson.computeCDF(lower - 2) < 0.99
assert poisson.computeCDF(upper) - poisson.computeCDF(lower - 1) >= 0.99

# The reduced-bias estimate averages the two thresholds around the
# Poisson move count M = ceil(-N log p).
move_count_estimate = int(math.ceil(-particle_count * math.log(tail_probability)))
ott.assert_almost_equal(
    estimate,
    0.5 * (thresholds[move_count_estimate - 2] + thresholds[move_count_estimate - 1]),
    1.0e-12,
    1.0e-12,
)
z = ot.DistFunc.qNormal(0.975)
# The reported interval contains the midpoint estimate, even when it lies
# beyond the Poisson rank interval.
lower_bound = min(estimate, thresholds[lower_rank - 1])
upper_bound = max(estimate, thresholds[upper_rank - 1])
ott.assert_almost_equal(interval.getLowerBound()[0], lower_bound, 1.0e-12, 1.0e-12)
ott.assert_almost_equal(interval.getUpperBound()[0], upper_bound, 1.0e-12, 1.0e-12)
ott.assert_almost_equal(
    result.getStandardError(), 0.5 * (upper_bound - lower_bound) / z, 1.0e-12, 1.0e-12
)

# ---------------------------------------------------------------------
# 3. Lower-tail symmetry
# ---------------------------------------------------------------------
ot.RandomGenerator.SetSeed(20260310)
algo = otexp.LastParticleQuantile(output, 0.05, 0.95)
algo.setParticleCount(particle_count)
algo.run()
lower_result = algo.getResult()
assert abs(lower_result.getQuantileEstimate() - true_lower) < 0.1
lower_interval = lower_result.getConfidenceInterval()
assert lower_interval.getLowerBound()[0] <= true_lower <= lower_interval.getUpperBound()[0]
assert lower_result.getTailType() == LOWER_TAIL
assert lower_result.getTailDescription() == "Lower tail"
lower_thresholds = point_values(lower_result.getThresholdSequence())
for previous, current in zip(lower_thresholds, lower_thresholds[1:]):
    assert current <= previous
assert all(math.isfinite(value) for value in lower_thresholds)

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
ot.RandomGenerator.SetSeed(20260311)
algo = otexp.LastParticleQuantile(composite_output, 0.95, 0.95)
algo.setParticleCount(500)
algo.run()
result = algo.getResult()
assert abs(result.getQuantileEstimate() - true_sum_quantile) < 0.25
assert result.getConfidenceInterval().getLowerBound()[0] <= true_sum_quantile
assert true_sum_quantile <= result.getConfidenceInterval().getUpperBound()[0]

# ---------------------------------------------------------------------
# 5. Persistence and copy
# ---------------------------------------------------------------------
ot.RandomGenerator.SetSeed(20260312)
algo = otexp.LastParticleQuantile(output, 0.95, 0.95)
algo.setParticleCount(400)
algo.setBurnIn(5)
algo.setProposalRange(1.5)
algo.run()
result = algo.getResult()

path = "last_particle_quantile.xml"
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager(path))
study.add("result", result)
study.add("algo", algo)
study.save()

study = ot.Study()
study.setStorageManager(ot.XMLStorageManager(path))
study.load()
loaded_algo = otexp.LastParticleQuantile()
study.fillObject("algo", loaded_algo)
loaded_result = otexp.QuantileSimulationResult()
study.fillObject("result", loaded_result)
os.remove(path)

assert loaded_algo.getParticleCount() == 400
assert loaded_algo.getBurnIn() == 5
assert loaded_algo.getProposalRange() == 1.5
assert loaded_algo.getResult().getQuantileEstimate() == result.getQuantileEstimate()
assert loaded_result.getQuantileEstimate() == result.getQuantileEstimate()
assert loaded_result.getStandardError() == result.getStandardError()
assert loaded_result.getConfidenceInterval() == result.getConfidenceInterval()
assert loaded_result.getStepCount() == result.getStepCount()
assert loaded_result.getThresholdSequence() == result.getThresholdSequence()
assert loaded_result.getAcceptanceRate() == result.getAcceptanceRate()
assert loaded_result.getPoissonRankLower() == result.getPoissonRankLower()
assert loaded_result.getPoissonRankUpper() == result.getPoissonRankUpper()
assert loaded_result.hasPoissonRanks()

copied = otexp.LastParticleQuantile(algo)
assert copied.getParticleCount() == 400
assert copied.getBurnIn() == 5
assert copied.getProposalRange() == 1.5

# ---------------------------------------------------------------------
# 6. Invalid arguments
# ---------------------------------------------------------------------
with ott.assert_raises(TypeError):
    otexp.LastParticleQuantile(output, 0.0, 0.95)
with ott.assert_raises(TypeError):
    otexp.LastParticleQuantile(output, 1.0, 0.95)
with ott.assert_raises(TypeError):
    otexp.LastParticleQuantile(output, 0.95, 0.0)
with ott.assert_raises(TypeError):
    otexp.LastParticleQuantile(output, 0.95, 1.0)
with ott.assert_raises(TypeError):
    otexp.LastParticleQuantile(ot.RandomVector(ot.Normal(2)), 0.95, 0.95)

algo = otexp.LastParticleQuantile(output)
with ott.assert_raises(TypeError):
    algo.setParticleCount(9)
with ott.assert_raises(TypeError):
    algo.setSampleSize(0)
with ott.assert_raises(TypeError):
    algo.setBurnIn(0)
with ott.assert_raises(TypeError):
    algo.setProposalRange(0.0)
with ott.assert_raises(TypeError):
    algo.setProposalRange(-1.0)
with ott.assert_raises(TypeError):
    algo.setProposalRange(float("inf"))

# The threshold estimator is an empirical inverse: reject the interpolated
# convention before running the particle process.
algo = otexp.LastParticleQuantile(output)
algo.setEstimatorType(INTERPOLATED)
with ott.assert_raises(TypeError):
    algo.run()

# A confidence level that puts the exact lower endpoint at infinity is
# rejected instead of silently truncating the lower Poisson rank.
algo = otexp.LastParticleQuantile(output, 0.5, 0.9999)
algo.setParticleCount(10)
with ott.assert_raises(TypeError):
    algo.run()

print("OK")

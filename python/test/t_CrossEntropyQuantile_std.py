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


# ---------------------------------------------------------------------
# 1. Defaults and accessors
# ---------------------------------------------------------------------
algo = otexp.CrossEntropyQuantile(output)
assert algo.getAlpha() == 0.95
assert algo.getConfidenceLevel() == 0.95
assert algo.getIntermediateQuantileLevel() == 0.25
assert algo.getPilotSize() == 1000
assert algo.getMinimumProposalStandardDeviation() == 1.2
assert algo.getSampleSize() == 1000
assert algo.getEstimatorType() == EMPIRICAL_INVERSE

algo.setIntermediateQuantileLevel(0.2)
algo.setPilotSize(500)
algo.setMinimumProposalStandardDeviation(1.5)
assert algo.getIntermediateQuantileLevel() == 0.2
assert algo.getPilotSize() == 500
assert algo.getMinimumProposalStandardDeviation() == 1.5
algo.setSampleSize(2000)
assert algo.getSampleSize() == 2000

# ---------------------------------------------------------------------
# 2. Upper-tail estimate and diagnostics
# ---------------------------------------------------------------------
sample_size = 3000
pilot_size = 500
minimum_sigma = 1.2
ot.RandomGenerator.SetSeed(20260315)
algo = otexp.CrossEntropyQuantile(output, 0.95, 0.95)
algo.setPilotSize(pilot_size)
algo.setSampleSize(sample_size)
algo.run()
result = algo.getResult()

estimate = result.getQuantileEstimate()
assert abs(estimate - true_upper) < 0.15
assert result.getStandardError() > 0.0
interval = result.getConfidenceInterval()
assert interval.getLowerBound()[0] <= true_upper <= interval.getUpperBound()[0]
assert result.getEstimatorType() == EMPIRICAL_INVERSE
assert result.getCoverageType() == COVERAGE_ASYMPTOTIC
assert result.getTailType() == UPPER_TAIL
assert result.getTailDescription() == "Upper tail"
assert result.getQuantileLevel() == 0.95
assert result.getIntermediateQuantileLevel() == 0.25
assert result.getParticleCount() == sample_size
assert result.getEffectiveSampleSize() > 0.0
assert result.getEffectiveSampleSize() <= sample_size
assert result.getSampleSize() >= pilot_size + sample_size
assert not result.isFixedSample()

# The result stores the defensive proposal: same dimension as the input,
# with every standard deviation floored at the minimum value.
assert result.hasAuxiliaryDistribution()
auxiliary = result.getAuxiliaryDistribution()
assert auxiliary.getDimension() == 1
assert all(
    sigma >= minimum_sigma - 1.0e-12
    for sigma in auxiliary.getStandardDeviation()
)
assert math.isfinite(auxiliary.getMean()[0])
assert result.getStepCount() == result.getThresholdSequence().getSize()
assert all(
    math.isfinite(result.getThresholdSequence()[i])
    for i in range(result.getThresholdSequence().getSize())
)

# ---------------------------------------------------------------------
# 3. The intermediate level is distinct from the target level
# ---------------------------------------------------------------------
ot.RandomGenerator.SetSeed(20260313)
algo = otexp.CrossEntropyQuantile(output, 0.99, 0.95)
algo.setIntermediateQuantileLevel(0.25)
algo.setPilotSize(pilot_size)
algo.setSampleSize(sample_size)
algo.run()
result = algo.getResult()
assert result.getQuantileLevel() == 0.99
assert result.getIntermediateQuantileLevel() == 0.25
assert abs(result.getQuantileEstimate() - distribution.computeQuantile(0.99)[0]) < 0.15
assert (
    result.getConfidenceInterval().getLowerBound()[0]
    <= distribution.computeQuantile(0.99)[0]
    <= result.getConfidenceInterval().getUpperBound()[0]
)

# ---------------------------------------------------------------------
# 4. Lower-tail symmetry
# ---------------------------------------------------------------------
ot.RandomGenerator.SetSeed(20260317)
algo = otexp.CrossEntropyQuantile(output, 0.05, 0.95)
algo.setPilotSize(pilot_size)
algo.setSampleSize(sample_size)
algo.run()
lower_result = algo.getResult()
assert abs(lower_result.getQuantileEstimate() - true_lower) < 0.15
lower_interval = lower_result.getConfidenceInterval()
assert lower_interval.getLowerBound()[0] <= true_lower <= lower_interval.getUpperBound()[0]
assert lower_result.getTailType() == LOWER_TAIL
assert lower_result.getTailDescription() == "Lower tail"

# ---------------------------------------------------------------------
# 5. Multidimensional input through a composite output
# ---------------------------------------------------------------------
input_distribution = ot.Normal(5)
input_vector = ot.RandomVector(input_distribution)
sum_function = ot.SymbolicFunction(
    [f"x{i}" for i in range(5)], ["x0 + x1 + x2 + x3 + x4"]
)
composite_output = ot.CompositeRandomVector(sum_function, input_vector)
true_sum_quantile = ot.Normal().computeQuantile(0.95)[0] * math.sqrt(5.0)
ot.RandomGenerator.SetSeed(20260316)
algo = otexp.CrossEntropyQuantile(composite_output, 0.95, 0.95)
algo.setPilotSize(pilot_size)
algo.setSampleSize(sample_size)
algo.run()
result = algo.getResult()
assert abs(result.getQuantileEstimate() - true_sum_quantile) < 0.2
assert result.getConfidenceInterval().getLowerBound()[0] <= true_sum_quantile
assert true_sum_quantile <= result.getConfidenceInterval().getUpperBound()[0]
assert result.getAuxiliaryDistribution().getDimension() == 5
assert all(
    sigma >= minimum_sigma - 1.0e-12
    for sigma in result.getAuxiliaryDistribution().getStandardDeviation()
)

# ---------------------------------------------------------------------
# 6. Persistence and copy
# ---------------------------------------------------------------------
ot.RandomGenerator.SetSeed(20260318)
algo = otexp.CrossEntropyQuantile(output, 0.95, 0.95)
algo.setIntermediateQuantileLevel(0.2)
algo.setPilotSize(500)
algo.setSampleSize(2000)
algo.setMinimumProposalStandardDeviation(1.5)
algo.run()
result = algo.getResult()

path = "cross_entropy_quantile.xml"
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager(path))
study.add("result", result)
study.add("algo", algo)
study.save()

study = ot.Study()
study.setStorageManager(ot.XMLStorageManager(path))
study.load()
loaded_algo = otexp.CrossEntropyQuantile()
study.fillObject("algo", loaded_algo)
loaded_result = otexp.QuantileSimulationResult()
study.fillObject("result", loaded_result)
os.remove(path)

assert loaded_algo.getIntermediateQuantileLevel() == 0.2
assert loaded_algo.getPilotSize() == 500
assert loaded_algo.getSampleSize() == 2000
assert loaded_algo.getMinimumProposalStandardDeviation() == 1.5
assert loaded_algo.getResult().getQuantileEstimate() == result.getQuantileEstimate()
assert loaded_result.getQuantileEstimate() == result.getQuantileEstimate()
assert loaded_result.getStandardError() == result.getStandardError()
assert loaded_result.getConfidenceInterval() == result.getConfidenceInterval()
assert loaded_result.getIntermediateQuantileLevel() == result.getIntermediateQuantileLevel()
assert loaded_result.getThresholdSequence() == result.getThresholdSequence()
assert loaded_result.getStepCount() == result.getStepCount()
assert loaded_result.hasAuxiliaryDistribution()
ott.assert_almost_equal(
    loaded_result.getAuxiliaryDistribution().getMean(),
    result.getAuxiliaryDistribution().getMean(),
    1.0e-12,
    1.0e-12,
)
ott.assert_almost_equal(
    loaded_result.getAuxiliaryDistribution().getStandardDeviation(),
    result.getAuxiliaryDistribution().getStandardDeviation(),
    1.0e-12,
    1.0e-12,
)

copied = otexp.CrossEntropyQuantile(algo)
assert copied.getIntermediateQuantileLevel() == 0.2
assert copied.getPilotSize() == 500
assert copied.getMinimumProposalStandardDeviation() == 1.5

# ---------------------------------------------------------------------
# 7. Invalid arguments
# ---------------------------------------------------------------------
with ott.assert_raises(TypeError):
    otexp.CrossEntropyQuantile(output, 0.0, 0.95)
with ott.assert_raises(TypeError):
    otexp.CrossEntropyQuantile(output, 1.0, 0.95)
with ott.assert_raises(TypeError):
    otexp.CrossEntropyQuantile(output, 0.95, 0.0)
with ott.assert_raises(TypeError):
    otexp.CrossEntropyQuantile(output, 0.95, 1.0)
with ott.assert_raises(TypeError):
    otexp.CrossEntropyQuantile(ot.RandomVector(ot.Normal(2)), 0.95, 0.95)

algo = otexp.CrossEntropyQuantile(output)
with ott.assert_raises(TypeError):
    algo.setIntermediateQuantileLevel(0.0)
with ott.assert_raises(TypeError):
    algo.setIntermediateQuantileLevel(1.0)
with ott.assert_raises(TypeError):
    algo.setPilotSize(1)
with ott.assert_raises(TypeError):
    algo.setMinimumProposalStandardDeviation(0.5)
with ott.assert_raises(TypeError):
    algo.setMinimumProposalStandardDeviation(float("inf"))
with ott.assert_raises(TypeError):
    algo.setSampleSize(0)

# The weighted estimator is an empirical inverse: reject the interpolated
# convention before running the expensive cross-entropy sequence.
algo = otexp.CrossEntropyQuantile(output)
algo.setEstimatorType(INTERPOLATED)
with ott.assert_raises(TypeError):
    algo.run()

algo = otexp.CrossEntropyQuantile(output)
algo.setSampleSize(1)
with ott.assert_raises(TypeError):
    algo.run()

# The outer stop callback applies to the whole run, including the pilot
# and final importance-sampling stages.
algo = otexp.CrossEntropyQuantile(output, 0.95, 0.95)
algo.setPilotSize(50)
algo.setSampleSize(50)
stop_calls = []


def stop():
    stop_calls.append(1)
    return len(stop_calls) >= 2


algo.setStopCallback(stop)
with ott.assert_raises(RuntimeError):
    algo.run()
assert stop_calls

print("OK")

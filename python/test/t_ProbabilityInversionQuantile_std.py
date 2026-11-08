#!/usr/bin/env python

import math
import os

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

EMPIRICAL_INVERSE = otexp.QuantileSimulationResultImplementation.EMPIRICAL_INVERSE
COVERAGE_EXACT = otexp.QuantileSimulationResultImplementation.COVERAGE_EXACT
COVERAGE_NONE = otexp.QuantileSimulationResultImplementation.COVERAGE_NONE
BISECTION = otexp.QuantileSimulationResultImplementation.BISECTION

normal = ot.Normal()
output = ot.RandomVector(normal)
true_upper = normal.computeQuantile(0.95)[0]

event = ot.ThresholdEvent(
    ot.CompositeRandomVector(ot.IdentityFunction(1), output),
    ot.Greater(),
    0.0,
)


def point_values(point):
    return [point[i] for i in range(point.getSize())]


def bracket_width(result):
    bracket = result.getRootBracket()
    return bracket.getUpperBound()[0] - bracket.getLowerBound()[0]


# ---------------------------------------------------------------------
# 1. Defaults and accessors
# ---------------------------------------------------------------------
algo = otexp.ProbabilityInversionQuantile()
assert algo.getAlpha() == 0.95
assert algo.getConfidenceLevel() == 0.95
assert algo.getEstimatorType() == EMPIRICAL_INVERSE
assert not algo.hasAlgorithm()
assert algo.getUseCommonRandomNumbers()
assert algo.getRootTolerance() == 1.0e-4
assert algo.getMaximumRootIteration() == 100
assert algo.getPilotSampleSize() == 1000
assert algo.getReplicateCount() == 1

algo = otexp.ProbabilityInversionQuantile(ot.ProbabilitySimulationAlgorithm(event))
assert algo.hasAlgorithm()
assert algo.getAlgorithm().getImplementation().getClassName() == "ProbabilitySimulationAlgorithm"

algo.setUseCommonRandomNumbers(False)
algo.setRootTolerance(1.0e-5)
algo.setMaximumRootIteration(50)
algo.setPilotSampleSize(500)
algo.setReplicateCount(3)
assert not algo.getUseCommonRandomNumbers()
assert algo.getRootTolerance() == 1.0e-5
assert algo.getMaximumRootIteration() == 50
assert algo.getPilotSampleSize() == 500
assert algo.getReplicateCount() == 3

# ---------------------------------------------------------------------
# 2. Plain Monte Carlo is delegated to direct extraction
# ---------------------------------------------------------------------
ot.RandomGenerator.SetSeed(20260417)
engine = ot.ProbabilitySimulationAlgorithm(event)
algo = otexp.ProbabilityInversionQuantile(engine, 0.95, 0.95)
algo.setSampleSize(20000)
algo.setPilotSampleSize(1000)
algo.run()
result = algo.getResult()

assert abs(result.getQuantileEstimate() - true_upper) < 0.1
assert result.getCoverageType() == COVERAGE_EXACT
assert result.getDelegatedToDirectExtraction()
assert not result.getCommonRandomNumbers()
assert not result.getRandomStreamCapability()
assert result.getEstimatorType() == EMPIRICAL_INVERSE
assert result.getConfidenceInterval().getLowerBound()[0] <= true_upper
assert true_upper <= result.getConfidenceInterval().getUpperBound()[0]
assert result.getWrappedAlgorithmDescription() != ""
assert result.getRootSolverType() == BISECTION

# ---------------------------------------------------------------------
# 3. Deterministic fixed-sample inversion recovers the order statistic
# ---------------------------------------------------------------------
ot.RandomGenerator.SetSeed(20260418)
sample = normal.getSample(201)
sorted_values = sorted([sample[i, 0] for i in range(201)])
expected_upper = sorted_values[int(math.ceil(0.95 * 201)) - 1]
expected_lower = sorted_values[int(math.ceil(0.05 * 201)) - 1]

engine = ot.ProbabilitySimulationAlgorithm(event, ot.FixedExperiment(sample))
algo = otexp.ProbabilityInversionQuantile(engine, 0.95, 0.95)
algo.setPilotSampleSize(1000)
algo.setRootTolerance(1.0e-10)
algo.run()
result = algo.getResult()
assert abs(result.getQuantileEstimate() - expected_upper) < 1.0e-8
assert result.getCoverageType() == COVERAGE_NONE
assert not result.getDelegatedToDirectExtraction()
assert not result.getCommonRandomNumbers()
assert not result.getRandomStreamCapability()
assert result.getRootIterationCount() > 0
assert result.getProbabilityEvaluationCount() > 0
assert bracket_width(result) < 1.0e-8

lower_event = ot.ThresholdEvent(
    ot.CompositeRandomVector(ot.IdentityFunction(1), output),
    ot.Less(),
    0.0,
)
engine = ot.ProbabilitySimulationAlgorithm(lower_event, ot.FixedExperiment(sample))
algo = otexp.ProbabilityInversionQuantile(engine, 0.05, 0.95)
algo.setPilotSampleSize(1000)
algo.setRootTolerance(1.0e-10)
algo.run()
result = algo.getResult()
assert abs(result.getQuantileEstimate() - expected_lower) < 1.0e-8
assert result.getCoverageType() == COVERAGE_NONE

# ---------------------------------------------------------------------
# 4. Non-delegated stochastic engine and replicate interval
# ---------------------------------------------------------------------
ot.RandomGenerator.SetSeed(20260419)
engine = ot.DirectionalSampling(event)
engine.setMaximumOuterSampling(200)
algo = otexp.ProbabilityInversionQuantile(engine, 0.95, 0.95)
algo.setPilotSampleSize(1000)
algo.setMaximumRootIteration(10)
algo.run()
result = algo.getResult()
assert math.isfinite(result.getQuantileEstimate())
assert result.getCoverageType() == COVERAGE_NONE
assert not result.getDelegatedToDirectExtraction()
assert not result.getCommonRandomNumbers()
assert not result.getRandomStreamCapability()
assert result.getRootIterationCount() > 0
assert result.getProbabilityEvaluationCount() > result.getRootIterationCount()
assert "DirectionalSampling" in result.getWrappedAlgorithmDescription()

ot.RandomGenerator.SetSeed(20260420)
engine = ot.DirectionalSampling(event)
engine.setMaximumOuterSampling(200)
algo = otexp.ProbabilityInversionQuantile(engine, 0.95, 0.95)
algo.setPilotSampleSize(500)
algo.setMaximumRootIteration(5)
algo.setReplicateCount(3)
algo.run()
result = algo.getResult()
assert result.getCoverageType() == COVERAGE_NONE
assert result.getReplicateCount() == 3
assert result.getReplicateEstimates().getSize() == 3
assert all(math.isfinite(value) for value in point_values(result.getReplicateEstimates()))

# ---------------------------------------------------------------------
# 5. Boundary root and bracket failure
# ---------------------------------------------------------------------
boundary_sample = ot.Sample(50, [1.0])
engine = ot.ProbabilitySimulationAlgorithm(
    event, ot.FixedExperiment(boundary_sample)
)
algo = otexp.ProbabilityInversionQuantile(engine, 0.95, 0.95)
algo.setPilotSampleSize(100)
algo.run()
result = algo.getResult()
assert abs(result.getQuantileEstimate() - 1.0) < 1.0e-3
assert result.getCoverageType() == COVERAGE_NONE

nan_sample = ot.Sample(10, 1)
for i in range(10):
    nan_sample[i, 0] = float("nan")
engine = ot.ProbabilitySimulationAlgorithm(event, ot.FixedExperiment(nan_sample))
algo = otexp.ProbabilityInversionQuantile(engine, 0.95, 0.95)
algo.setPilotSampleSize(100)
with ott.assert_raises(RuntimeError):
    algo.run()

# ---------------------------------------------------------------------
# 6. Persistence and copy
# ---------------------------------------------------------------------
ot.RandomGenerator.SetSeed(20260421)
engine = ot.ProbabilitySimulationAlgorithm(event)
algo = otexp.ProbabilityInversionQuantile(engine, 0.95, 0.95)
algo.setSampleSize(5000)
algo.setPilotSampleSize(500)
algo.setReplicateCount(2)
algo.run()
result = algo.getResult()

path = "probability_inversion_quantile.xml"
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager(path))
study.add("result", result)
study.add("algo", algo)
study.save()

study = ot.Study()
study.setStorageManager(ot.XMLStorageManager(path))
study.load()
loaded_algo = otexp.ProbabilityInversionQuantile()
study.fillObject("algo", loaded_algo)
loaded_result = otexp.QuantileSimulationResult()
study.fillObject("result", loaded_result)
os.remove(path)

assert loaded_algo.hasAlgorithm()
assert loaded_algo.getSampleSize() == 5000
assert loaded_algo.getPilotSampleSize() == 500
assert loaded_algo.getReplicateCount() == 2
assert loaded_algo.getUseCommonRandomNumbers() == algo.getUseCommonRandomNumbers()
assert loaded_result.getQuantileEstimate() == result.getQuantileEstimate()
assert loaded_result.getCoverageType() == result.getCoverageType()
assert (
    loaded_result.getDelegatedToDirectExtraction()
    == result.getDelegatedToDirectExtraction()
)
assert (
    loaded_result.getWrappedAlgorithmDescription()
    == result.getWrappedAlgorithmDescription()
)
assert loaded_result.getRootBracket() == result.getRootBracket()
assert (
    loaded_result.getProbabilityEvaluationCount()
    == result.getProbabilityEvaluationCount()
)
assert (
    loaded_result.getReplicateEstimates()
    == result.getReplicateEstimates()
)

copied = otexp.ProbabilityInversionQuantile(algo)
assert copied.hasAlgorithm()
assert copied.getSampleSize() == 5000
assert copied.getPilotSampleSize() == 500
assert copied.getReplicateCount() == 2

# ---------------------------------------------------------------------
# 7. Invalid arguments
# ---------------------------------------------------------------------
with ott.assert_raises(TypeError):
    otexp.ProbabilityInversionQuantile(engine, 0.0, 0.95)
with ott.assert_raises(TypeError):
    otexp.ProbabilityInversionQuantile(engine, 1.0, 0.95)
with ott.assert_raises(TypeError):
    otexp.ProbabilityInversionQuantile(engine, 0.95, 0.0)
with ott.assert_raises(TypeError):
    otexp.ProbabilityInversionQuantile(engine, 0.95, 1.0)

algo = otexp.ProbabilityInversionQuantile()
with ott.assert_raises(TypeError):
    algo.setRootTolerance(0.0)
with ott.assert_raises(TypeError):
    algo.setRootTolerance(float("inf"))
with ott.assert_raises(TypeError):
    algo.setMaximumRootIteration(0)
with ott.assert_raises(TypeError):
    algo.setPilotSampleSize(0)
with ott.assert_raises(TypeError):
    algo.setReplicateCount(0)
with ott.assert_raises(TypeError):
    algo.run()

equal_event = ot.ThresholdEvent(
    ot.CompositeRandomVector(ot.IdentityFunction(1), output), ot.Equal(), 0.0
)
with ott.assert_raises(TypeError):
    algo.setAlgorithm(ot.ProbabilitySimulationAlgorithm(equal_event))

print("OK")

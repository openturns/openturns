#!/usr/bin/env python

import os

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

EMPIRICAL_INVERSE = otexp.QuantileSimulationResultImplementation.EMPIRICAL_INVERSE
COVERAGE_EXACT = otexp.QuantileSimulationResultImplementation.COVERAGE_EXACT
COVERAGE_ANYTIME_VALID = (
    otexp.QuantileSimulationResultImplementation.COVERAGE_ANYTIME_VALID
)

distribution = ot.Normal()
output = ot.RandomVector(distribution)
true_quantile = distribution.computeQuantile(0.95)[0]
true_quantile_09 = distribution.computeQuantile(0.9)[0]


# ---------------------------------------------------------------------
# 1. Defaults, accessors and invalid arguments
# ---------------------------------------------------------------------
algo = otexp.SequentialQuantileSimulation(output)
assert algo.getAlpha() == 0.95
assert algo.getConfidenceLevel() == 0.95
assert algo.getSampleSize() == 0
assert algo.getEstimatorType() == EMPIRICAL_INVERSE
assert not algo.hasTargetHalfWidth()
assert algo.getTargetHalfWidth() == 0.0

algo.setSampleSize(5000)
algo.setTargetHalfWidth(0.2)
assert algo.getSampleSize() == 5000
assert algo.hasTargetHalfWidth()
assert algo.getTargetHalfWidth() == 0.2

with ott.assert_raises(TypeError):
    algo.setTargetHalfWidth(0.0)
with ott.assert_raises(TypeError):
    algo.setTargetHalfWidth(-1.0)
with ott.assert_raises(TypeError):
    algo.setTargetHalfWidth(float("inf"))
with ott.assert_raises(TypeError):
    algo.setSampleSize(0)

empty = otexp.SequentialQuantileSimulation()
with ott.assert_raises(TypeError):
    empty.run()

# ---------------------------------------------------------------------
# 2. Nominal run: the confidence sequence stops on the target width
# ---------------------------------------------------------------------
ot.RandomGenerator.SetSeed(20261108)
algo = otexp.SequentialQuantileSimulation(output, 0.95, 0.95)
algo.setSampleSize(100000)
algo.setTargetHalfWidth(0.2)
assert algo.getSampleSize() == 100000
algo.run()
result = algo.getResult()

estimate = result.getQuantileEstimate()
stopping_time = result.getStoppingTime()
assert result.getCoverageType() == COVERAGE_ANYTIME_VALID
assert result.getEstimatorType() == EMPIRICAL_INVERSE
assert not result.isFixedSample()
assert result.getStoppingCriterion() == "target half-width"
assert stopping_time < 100000
assert result.getSampleSize() == stopping_time
assert result.getEffectiveSampleSize() == stopping_time
assert not result.getBudgetExhausted()
assert result.hasTargetHalfWidth()
assert result.getTargetHalfWidth() == 0.2
assert len(result.getCoverageAssumption()) > 0
assert abs(estimate - true_quantile) < 0.3
assert result.getStandardError() > 0.0

interval = result.getConfidenceInterval()
assert interval.getLowerBound()[0] <= true_quantile <= interval.getUpperBound()[0]
half_width = 0.5 * (
    interval.getUpperBound()[0] - interval.getLowerBound()[0]
)
assert half_width <= 0.2

path = result.getConfidenceSequencePath()
assert path.getSize() >= 2
assert path.getDimension() == 4
assert path[-1, 0] == stopping_time
assert path[-1, 1] == estimate
assert path[-1, 2] == interval.getLowerBound()[0]
assert path[-1, 3] == interval.getUpperBound()[0]
for i in range(1, path.getSize()):
    assert path[i, 0] > path[i - 1, 0]
assert result.getConvergenceHistory() == path
assert result.drawQuantileConvergence().getDrawable(0).getData().getSize() > 1

# ---------------------------------------------------------------------
# 3. Budget exhaustion: the interval widens to the last available size
# ---------------------------------------------------------------------
ot.RandomGenerator.SetSeed(20261109)
algo = otexp.SequentialQuantileSimulation(output, 0.95, 0.95)
algo.setSampleSize(5000)
algo.setTargetHalfWidth(0.01)
algo.run()
result = algo.getResult()
assert result.getStoppingTime() == 5000
assert result.getStoppingCriterion() == "maximum sample size"
assert result.getBudgetExhausted()
assert result.getSampleSize() == 5000
assert result.getCoverageType() == COVERAGE_ANYTIME_VALID
interval = result.getConfidenceInterval()
assert interval.getLowerBound()[0] <= true_quantile <= interval.getUpperBound()[0]
assert interval.getUpperBound()[0] < ot.SpecFunc.MaxScalar

# ---------------------------------------------------------------------
# 4. No target: the run stops at the evaluation budget
# ---------------------------------------------------------------------
ot.RandomGenerator.SetSeed(20261110)
algo = otexp.SequentialQuantileSimulation(output, 0.95, 0.95)
algo.setSampleSize(3000)
algo.run()
result = algo.getResult()
assert result.getStoppingTime() == 3000
assert result.getStoppingCriterion() == "maximum sample size"
assert not result.getBudgetExhausted()
assert not result.hasTargetHalfWidth()
assert result.getCoverageType() == COVERAGE_ANYTIME_VALID

# ---------------------------------------------------------------------
# 5. Anytime-valid coverage with random stopping times
# ---------------------------------------------------------------------
repetition_count = 200
covered = 0
stopping_times = set()
ot.RandomGenerator.SetSeed(20261111)
for _ in range(repetition_count):
    algo = otexp.SequentialQuantileSimulation(output, 0.9, 0.8)
    algo.setSampleSize(8000)
    algo.setTargetHalfWidth(0.3)
    algo.run()
    result = algo.getResult()
    stopping_times.add(result.getStoppingTime())
    interval = result.getConfidenceInterval()
    if interval.getLowerBound()[0] <= true_quantile_09 <= interval.getUpperBound()[0]:
        covered += 1
coverage = covered / repetition_count
assert len(stopping_times) > 1
assert coverage >= 0.72

# ---------------------------------------------------------------------
# 6. Invalid ResourceMap values and required coverage
# ---------------------------------------------------------------------
grid_ratio = ot.ResourceMap.GetAsScalar("SequentialQuantileSimulation-GridRatio")
try:
    ot.ResourceMap.SetAsScalar("SequentialQuantileSimulation-GridRatio", 1.0)
    algo = otexp.SequentialQuantileSimulation(output, 0.95, 0.95)
    algo.setSampleSize(1000)
    with ott.assert_raises(TypeError):
        algo.run()
finally:
    ot.ResourceMap.SetAsScalar("SequentialQuantileSimulation-GridRatio", grid_ratio)

algo = otexp.SequentialQuantileSimulation(output, 0.9, 0.9)
algo.setSampleSize(500)
algo.setRequiredCoverage(COVERAGE_EXACT)
assert algo.getRequiredCoverage() == COVERAGE_EXACT
with ott.assert_raises(TypeError):
    algo.run()

# ---------------------------------------------------------------------
# 7. Persistence
# ---------------------------------------------------------------------
ot.RandomGenerator.SetSeed(20261112)
algo = otexp.SequentialQuantileSimulation(output, 0.9, 0.9)
algo.setSampleSize(4000)
algo.setTargetHalfWidth(0.35)
algo.run()
result = algo.getResult()

path = os.path.join(os.getcwd(), "t_SequentialQuantileSimulation_std.xml")
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager(path))
study.add("result", result)
study.add("algo", algo)
study.save()

study = ot.Study()
study.setStorageManager(ot.XMLStorageManager(path))
study.load()
loaded_algo = otexp.SequentialQuantileSimulation()
study.fillObject("algo", loaded_algo)
loaded_result = otexp.QuantileSimulationResult()
study.fillObject("result", loaded_result)
os.remove(path)

assert loaded_algo.getSampleSize() == 4000
assert loaded_algo.hasTargetHalfWidth()
assert loaded_algo.getTargetHalfWidth() == 0.35
assert loaded_result.getStoppingTime() == result.getStoppingTime()
assert loaded_result.getStoppingCriterion() == result.getStoppingCriterion()
assert loaded_result.getQuantileEstimate() == result.getQuantileEstimate()
assert loaded_result.getBudgetExhausted() == result.getBudgetExhausted()
assert loaded_result.getCoverageAssumption() == result.getCoverageAssumption()
assert (
    loaded_result.getConfidenceSequencePath().getSize()
    == result.getConfidenceSequencePath().getSize()
)
assert loaded_result.getCoverageType() == result.getCoverageType()

copied = otexp.SequentialQuantileSimulation(algo)
assert copied.getSampleSize() == 4000
assert copied.hasTargetHalfWidth()

print("OK")

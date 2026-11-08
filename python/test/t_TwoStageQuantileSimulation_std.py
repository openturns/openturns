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
minimum_size = ot.QuantileConfidence(0.95, 0.95).computeBilateralMinimumSampleSize()


# ---------------------------------------------------------------------
# 1. Defaults, accessors and invalid arguments
# ---------------------------------------------------------------------
algo = otexp.TwoStageQuantileSimulation(output)
assert algo.getAlpha() == 0.95
assert algo.getConfidenceLevel() == 0.95
assert algo.getSampleSize() == 0
assert algo.getEstimatorType() == EMPIRICAL_INVERSE
assert algo.getPilotSize() == 1000
assert not algo.hasTargetHalfWidth()
assert algo.getTargetHalfWidth() == 0.0

algo.setPilotSize(400)
algo.setTargetHalfWidth(0.1)
assert algo.getPilotSize() == 400
assert algo.hasTargetHalfWidth()
assert algo.getTargetHalfWidth() == 0.1

algo.setSampleSize(5000)
assert algo.getSampleSize() == 5000
algo.setRequiredCoverage(COVERAGE_EXACT)
assert algo.getRequiredCoverage() == COVERAGE_EXACT

with ott.assert_raises(TypeError):
    algo.setPilotSize(0)
with ott.assert_raises(TypeError):
    algo.setTargetHalfWidth(0.0)
with ott.assert_raises(TypeError):
    algo.setTargetHalfWidth(-1.0)
with ott.assert_raises(TypeError):
    algo.setTargetHalfWidth(float("inf"))

empty = otexp.TwoStageQuantileSimulation()
with ott.assert_raises(TypeError):
    empty.run()

# ---------------------------------------------------------------------
# 2. Nominal run: pilot sizing and exact confirmation interval
# ---------------------------------------------------------------------
ot.RandomGenerator.SetSeed(20261108)
algo = otexp.TwoStageQuantileSimulation(output, 0.95, 0.95)
algo.setSampleSize(6000)
algo.setPilotSize(500)
algo.setTargetHalfWidth(0.1)
assert algo.getSampleSize() == 6000
algo.run()
result = algo.getResult()

estimate = result.getQuantileEstimate()
assert abs(estimate - true_quantile) < 0.15
assert result.getStandardError() > 0.0
interval = result.getConfidenceInterval()
assert interval.getLowerBound()[0] <= true_quantile <= interval.getUpperBound()[0]

assert result.getCoverageType() == COVERAGE_EXACT
assert result.getEstimatorType() == EMPIRICAL_INVERSE
assert result.isFixedSample()
assert result.getPilotSize() == 500
confirmation_size = result.getConfirmationSize()
assert confirmation_size >= minimum_size
assert result.getRequiredConfirmationSize() >= minimum_size
assert result.getSampleSize() == 500 + confirmation_size
assert result.getEffectiveSampleSize() == confirmation_size
assert result.hasPilotQuantileEstimate()
assert result.hasPilotDensity()
assert result.getPilotDensity() > 0.0
assert result.hasTargetHalfWidth()
assert result.getTargetHalfWidth() == 0.1
assert not result.getBudgetExhausted()
half_width = 0.5 * (
    interval.getUpperBound()[0] - interval.getLowerBound()[0]
)
assert half_width <= 0.15

history = result.getConvergenceHistory()
assert history.getSize() == 1
assert history.getDimension() == 4
assert history[0, 0] == 500 + confirmation_size
assert history[0, 2] == interval.getLowerBound()[0]
assert history[0, 3] == interval.getUpperBound()[0]

# ---------------------------------------------------------------------
# 3. No target: the confirmation stage uses the remaining budget
# ---------------------------------------------------------------------
ot.RandomGenerator.SetSeed(20261109)
algo = otexp.TwoStageQuantileSimulation(output, 0.95, 0.95)
algo.setSampleSize(2000)
algo.setPilotSize(400)
algo.run()
result = algo.getResult()
assert result.getPilotSize() == 400
assert result.getConfirmationSize() == 1600
assert result.getRequiredConfirmationSize() == 1600
assert not result.hasTargetHalfWidth()
assert not result.getBudgetExhausted()
assert result.getCoverageType() == COVERAGE_EXACT
assert abs(result.getQuantileEstimate() - true_quantile) < 0.15

# ---------------------------------------------------------------------
# 4. Budget exhaustion: the interval stays exact at the achieved size
# ---------------------------------------------------------------------
ot.RandomGenerator.SetSeed(20261110)
algo = otexp.TwoStageQuantileSimulation(output, 0.9, 0.9)
algo.setSampleSize(300)
algo.setPilotSize(200)
algo.setTargetHalfWidth(1.0e-6)
algo.run()
result = algo.getResult()
assert result.getPilotSize() == 200
assert result.getConfirmationSize() == 100
assert result.getRequiredConfirmationSize() > result.getConfirmationSize()
assert result.getBudgetExhausted()
assert result.getSampleSize() == 300
assert result.getCoverageType() == COVERAGE_EXACT
assert result.isFixedSample()

# A budget below the minimum exact interval size is rejected.
algo = otexp.TwoStageQuantileSimulation(output, 0.99, 0.99)
algo.setSampleSize(100)
algo.setPilotSize(50)
with ott.assert_raises(TypeError):
    algo.run()

# A required coverage that the design cannot achieve is rejected.
algo = otexp.TwoStageQuantileSimulation(output, 0.9, 0.9)
algo.setSampleSize(300)
algo.setPilotSize(200)
algo.setRequiredCoverage(COVERAGE_ANYTIME_VALID)
with ott.assert_raises(TypeError):
    algo.run()

# ---------------------------------------------------------------------
# 5. Unconditional coverage with pilot randomness
# ---------------------------------------------------------------------
repetition_count = 300
pilot_size = 100
target_half_width = 0.2
covered = 0
confirmation_sizes = set()
ot.RandomGenerator.SetSeed(20261111)
for _ in range(repetition_count):
    algo = otexp.TwoStageQuantileSimulation(output, 0.9, 0.9)
    algo.setSampleSize(5000)
    algo.setPilotSize(pilot_size)
    algo.setTargetHalfWidth(target_half_width)
    algo.run()
    result = algo.getResult()
    confirmation_sizes.add(result.getConfirmationSize())
    interval = result.getConfidenceInterval()
    if interval.getLowerBound()[0] <= true_quantile_09 <= interval.getUpperBound()[0]:
        covered += 1
coverage = covered / repetition_count
assert len(confirmation_sizes) > 1
assert coverage >= 0.85

# ---------------------------------------------------------------------
# 6. Persistence
# ---------------------------------------------------------------------
ot.RandomGenerator.SetSeed(20261112)
algo = otexp.TwoStageQuantileSimulation(output, 0.95, 0.95)
algo.setSampleSize(2000)
algo.setPilotSize(400)
algo.setTargetHalfWidth(0.2)
algo.run()
result = algo.getResult()

path = os.path.join(os.getcwd(), "t_TwoStageQuantileSimulation_std.xml")
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager(path))
study.add("result", result)
study.add("algo", algo)
study.save()

study = ot.Study()
study.setStorageManager(ot.XMLStorageManager(path))
study.load()
loaded_algo = otexp.TwoStageQuantileSimulation()
study.fillObject("algo", loaded_algo)
loaded_result = otexp.QuantileSimulationResult()
study.fillObject("result", loaded_result)
os.remove(path)

assert loaded_algo.getSampleSize() == 2000
assert loaded_algo.getPilotSize() == 400
assert loaded_algo.hasTargetHalfWidth()
assert loaded_algo.getTargetHalfWidth() == 0.2
assert loaded_result.getPilotSize() == result.getPilotSize()
assert loaded_result.getConfirmationSize() == result.getConfirmationSize()
assert (
    loaded_result.getRequiredConfirmationSize()
    == result.getRequiredConfirmationSize()
)
assert loaded_result.getQuantileEstimate() == result.getQuantileEstimate()
assert loaded_result.getBudgetExhausted() == result.getBudgetExhausted()
assert loaded_result.getCoverageType() == result.getCoverageType()

copied = otexp.TwoStageQuantileSimulation(algo)
assert copied.getSampleSize() == 2000
assert copied.getPilotSize() == 400
assert copied.hasTargetHalfWidth()

print("OK")

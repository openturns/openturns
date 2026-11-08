#!/usr/bin/env python

import math
import os

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

EMPIRICAL_INVERSE = otexp.QuantileSimulationResultImplementation.EMPIRICAL_INVERSE
COVERAGE_ASYMPTOTIC = otexp.QuantileSimulationResultImplementation.COVERAGE_ASYMPTOTIC
BISECTION = otexp.QuantileSimulationResultImplementation.BISECTION

normal = ot.Normal()
output = ot.RandomVector(normal)
true_upper = normal.computeQuantile(0.95)[0]
true_lower = normal.computeQuantile(0.05)[0]


def point_values(point):
    return [point[i] for i in range(point.getSize())]


def bracket_width(result):
    bracket = result.getRootBracket()
    return bracket.getUpperBound()[0] - bracket.getLowerBound()[0]


# ---------------------------------------------------------------------
# 1. Defaults and accessors
# ---------------------------------------------------------------------
algo = otexp.LineSamplingQuantile(output)
assert algo.getAlpha() == 0.95
assert algo.getConfidenceLevel() == 0.95
assert algo.getEstimatorType() == EMPIRICAL_INVERSE
assert algo.getInitialAlpha().getSize() == 0
assert algo.getRootStrategy().getImplementation().getClassName() == "SafeAndSlow"
assert algo.getSearchOppositeDirection()
assert algo.getRootTolerance() == 1.0e-4
assert algo.getMaximumRootIteration() == 100
assert algo.getPilotSampleSize() == 1000

algo.setInitialAlpha([1.0, 1.0])
algo.setRootStrategy(ot.MediumSafe())
algo.setSearchOppositeDirection(False)
algo.setRootTolerance(1.0e-5)
algo.setMaximumRootIteration(50)
algo.setPilotSampleSize(500)
algo.setSampleSize(2000)
assert point_values(algo.getInitialAlpha()) == [1.0, 1.0]
assert algo.getRootStrategy().getImplementation().getClassName() == "MediumSafe"
assert not algo.getSearchOppositeDirection()
assert algo.getRootTolerance() == 1.0e-5
assert algo.getMaximumRootIteration() == 50
assert algo.getPilotSampleSize() == 500
assert algo.getSampleSize() == 2000

# ---------------------------------------------------------------------
# 2. Upper-tail estimate and root diagnostics on a linear limit state
# ---------------------------------------------------------------------
ot.RandomGenerator.SetSeed(20260413)
sample_size = 4000
algo = otexp.LineSamplingQuantile(output, 0.95, 0.95)
algo.setSampleSize(sample_size)
algo.setPilotSampleSize(1000)
algo.setInitialAlpha([1.0])
algo.setRootTolerance(1.0e-6)
algo.run()
result = algo.getResult()

estimate = result.getQuantileEstimate()
assert abs(estimate - true_upper) < 1.0e-4
assert result.getStandardError() >= 0.0
interval = result.getConfidenceInterval()
assert interval.getLowerBound()[0] <= true_upper <= interval.getUpperBound()[0]
assert result.getEstimatorType() == EMPIRICAL_INVERSE
assert result.getCoverageType() == COVERAGE_ASYMPTOTIC
assert point_values(result.getImportantDirection()) == [1.0]
assert result.hasImportantDirection()
assert result.getConditionalProbabilitySample().getSize() == sample_size
assert all(
    0.0 <= value <= 1.0
    for value in point_values(result.getConditionalProbabilitySample())
)
assert result.getRootIterationCount() > 0
assert result.getProbabilityEvaluationCount() > result.getRootIterationCount()
assert bracket_width(result) < 1.0e-5
assert result.getRootSolverType() == BISECTION
assert result.getRootSolverDescription() == "Bisection"
assert result.getCommonRandomNumbers()
assert not result.getRandomStreamCapability()
assert not result.getDelegatedToDirectExtraction()
assert result.isFixedSample()
assert result.getSampleSize() == sample_size

# ---------------------------------------------------------------------
# 3. Multidimensional linear limit state with an automatic direction
# ---------------------------------------------------------------------
dimension = 5
input_distribution = ot.Normal(dimension)
input_vector = ot.RandomVector(input_distribution)
sum_function = ot.SymbolicFunction(
    [f"x{i}" for i in range(dimension)],
    ["+".join(f"x{i}" for i in range(dimension))],
)
composite_output = ot.CompositeRandomVector(sum_function, input_vector)
true_sum_quantile = true_upper * math.sqrt(dimension)

ot.RandomGenerator.SetSeed(20260414)
algo = otexp.LineSamplingQuantile(composite_output, 0.95, 0.95)
algo.setSampleSize(4000)
algo.setPilotSampleSize(1000)
algo.setRootTolerance(1.0e-6)
algo.run()
result = algo.getResult()
assert abs(result.getQuantileEstimate() - true_sum_quantile) < 1.0e-3
assert bracket_width(result) < 1.0e-5
direction = result.getImportantDirection()
assert direction.getSize() == dimension
ott.assert_almost_equal(
    math.sqrt(sum(value * value for value in point_values(direction))),
    1.0,
    1.0e-12,
    1.0e-12,
)
for value in point_values(direction):
    ott.assert_almost_equal(value, 1.0 / math.sqrt(dimension), 1.0e-4, 1.0e-4)

# The opposite-direction search must not change the linear estimate.
ot.RandomGenerator.SetSeed(20260415)
algo = otexp.LineSamplingQuantile(composite_output, 0.95, 0.95)
algo.setSampleSize(4000)
algo.setPilotSampleSize(1000)
algo.setSearchOppositeDirection(False)
algo.setRootTolerance(1.0e-6)
algo.run()
assert abs(algo.getResult().getQuantileEstimate() - true_sum_quantile) < 1.0e-3

# ---------------------------------------------------------------------
# 4. Lower-tail symmetry
# ---------------------------------------------------------------------
ot.RandomGenerator.SetSeed(20260416)
algo = otexp.LineSamplingQuantile(output, 0.05, 0.95)
algo.setSampleSize(sample_size)
algo.setPilotSampleSize(1000)
algo.setInitialAlpha([1.0])
algo.setRootTolerance(1.0e-6)
algo.run()
lower_result = algo.getResult()
assert abs(lower_result.getQuantileEstimate() - true_lower) < 1.0e-4
assert lower_result.getCoverageType() == COVERAGE_ASYMPTOTIC

# ---------------------------------------------------------------------
# 5. Persistence and copy
# ---------------------------------------------------------------------
ot.RandomGenerator.SetSeed(20260417)
algo = otexp.LineSamplingQuantile(output, 0.95, 0.95)
algo.setSampleSize(500)
algo.setPilotSampleSize(200)
algo.setInitialAlpha([1.0])
algo.setSearchOppositeDirection(False)
algo.setRootTolerance(1.0e-6)
algo.setMaximumRootIteration(50)
algo.run()
result = algo.getResult()

path = "line_sampling_quantile.xml"
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager(path))
study.add("result", result)
study.add("algo", algo)
study.save()

study = ot.Study()
study.setStorageManager(ot.XMLStorageManager(path))
study.load()
loaded_algo = otexp.LineSamplingQuantile()
study.fillObject("algo", loaded_algo)
loaded_result = otexp.QuantileSimulationResult()
study.fillObject("result", loaded_result)
os.remove(path)

assert loaded_algo.getSampleSize() == 500
assert loaded_algo.getPilotSampleSize() == 200
assert point_values(loaded_algo.getInitialAlpha()) == [1.0]
assert not loaded_algo.getSearchOppositeDirection()
assert loaded_algo.getRootTolerance() == 1.0e-6
assert loaded_algo.getMaximumRootIteration() == 50
assert loaded_result.getQuantileEstimate() == result.getQuantileEstimate()
assert loaded_result.getStandardError() == result.getStandardError()
assert loaded_result.getConfidenceInterval() == result.getConfidenceInterval()
assert loaded_result.getRootBracket() == result.getRootBracket()
assert loaded_result.getImportantDirection() == result.getImportantDirection()
assert loaded_result.hasImportantDirection()
assert loaded_result.getRootIterationCount() == result.getRootIterationCount()
assert (
    loaded_result.getProbabilityEvaluationCount()
    == result.getProbabilityEvaluationCount()
)
assert (
    loaded_result.getConditionalProbabilitySample()
    == result.getConditionalProbabilitySample()
)
assert loaded_result.getCommonRandomNumbers() == result.getCommonRandomNumbers()
assert loaded_result.getCoverageType() == result.getCoverageType()

copied = otexp.LineSamplingQuantile(algo)
assert copied.getSampleSize() == 500
assert point_values(copied.getInitialAlpha()) == [1.0]
assert not copied.getSearchOppositeDirection()

# ---------------------------------------------------------------------
# 6. Invalid arguments
# ---------------------------------------------------------------------
with ott.assert_raises(TypeError):
    otexp.LineSamplingQuantile(output, 0.0, 0.95)
with ott.assert_raises(TypeError):
    otexp.LineSamplingQuantile(output, 1.0, 0.95)
with ott.assert_raises(TypeError):
    otexp.LineSamplingQuantile(output, 0.95, 0.0)
with ott.assert_raises(TypeError):
    otexp.LineSamplingQuantile(output, 0.95, 1.0)
with ott.assert_raises(TypeError):
    otexp.LineSamplingQuantile(ot.RandomVector(ot.Normal(2)), 0.95, 0.95)

algo = otexp.LineSamplingQuantile(output)
with ott.assert_raises(TypeError):
    algo.setInitialAlpha([0.0])
with ott.assert_raises(TypeError):
    algo.setInitialAlpha([float("inf")])
with ott.assert_raises(TypeError):
    algo.setRootTolerance(0.0)
with ott.assert_raises(TypeError):
    algo.setRootTolerance(float("inf"))
with ott.assert_raises(TypeError):
    algo.setMaximumRootIteration(0)
with ott.assert_raises(TypeError):
    algo.setPilotSampleSize(0)
with ott.assert_raises(TypeError):
    algo.setSampleSize(0)

algo = otexp.LineSamplingQuantile(output)
algo.setInitialAlpha([1.0, 1.0])
algo.setSampleSize(100)
with ott.assert_raises(TypeError):
    algo.run()

algo = otexp.LineSamplingQuantile(output)
algo.setSampleSize(1)
with ott.assert_raises(TypeError):
    algo.run()

print("OK")

#!/usr/bin/env python

import math
import os

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

EMPIRICAL_INVERSE = otexp.QuantileSimulationResultImplementation.EMPIRICAL_INVERSE
COVERAGE_NONE = otexp.QuantileSimulationResultImplementation.COVERAGE_NONE
COVERAGE_ASYMPTOTIC = otexp.QuantileSimulationResultImplementation.COVERAGE_ASYMPTOTIC
BISECTION = otexp.QuantileSimulationResultImplementation.BISECTION
FORM_APPROXIMATION = otexp.PostAnalyticalQuantile.FORM_APPROXIMATION
SORM_APPROXIMATION = otexp.PostAnalyticalQuantile.SORM_APPROXIMATION
BREITUNG = otexp.PostAnalyticalQuantile.BREITUNG
HOHENBICHLER = otexp.PostAnalyticalQuantile.HOHENBICHLER
TVEDT = otexp.PostAnalyticalQuantile.TVEDT

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
algo = otexp.PostAnalyticalQuantile(output)
assert algo.getAlpha() == 0.95
assert algo.getConfidenceLevel() == 0.95
assert algo.getEstimatorType() == EMPIRICAL_INVERSE
assert algo.getAnalyticalMethod() == FORM_APPROXIMATION
assert algo.getAnalyticalMethodDescription() == "FORM"
assert algo.getSORMFormula() == HOHENBICHLER
assert algo.getSORMFormulaDescription() == "Hohenbichler"
assert not algo.getRefinementEnabled()
assert algo.getReplicateCount() == 30
assert algo.getNearestPointAlgorithm().getImplementation().getClassName() == "AbdoRackwitz"
assert algo.getRootTolerance() == 1.0e-4
assert algo.getMaximumRootIteration() == 100
assert algo.getPilotSampleSize() == 1000

algo.setAnalyticalMethod(SORM_APPROXIMATION)
algo.setSORMFormula(BREITUNG)
algo.setRefinementEnabled(True)
algo.setReplicateCount(12)
algo.setNearestPointAlgorithm(ot.Cobyla())
algo.setRootTolerance(1.0e-5)
algo.setMaximumRootIteration(50)
algo.setPilotSampleSize(500)
algo.setSampleSize(2000)
assert algo.getAnalyticalMethod() == SORM_APPROXIMATION
assert algo.getAnalyticalMethodDescription() == "SORM"
assert algo.getSORMFormula() == BREITUNG
assert algo.getSORMFormulaDescription() == "Breitung"
assert algo.getRefinementEnabled()
assert algo.getReplicateCount() == 12
assert algo.getNearestPointAlgorithm().getImplementation().getClassName() == "Cobyla"
assert algo.getRootTolerance() == 1.0e-5
assert algo.getMaximumRootIteration() == 50
assert algo.getPilotSampleSize() == 500
assert algo.getSampleSize() == 2000

for formula, description in [
    (BREITUNG, "Breitung"),
    (HOHENBICHLER, "Hohenbichler"),
    (TVEDT, "Tvedt"),
]:
    algo.setSORMFormula(formula)
    assert algo.getSORMFormula() == formula
    assert algo.getSORMFormulaDescription() == description

# ---------------------------------------------------------------------
# 2. FORM quantile on a linear limit state
# ---------------------------------------------------------------------
algo = otexp.PostAnalyticalQuantile(output, 0.95, 0.95)
algo.setRootTolerance(1.0e-9)
algo.run()
result = algo.getResult()

assert result.getSampleSize() == algo.getPilotSampleSize()
assert result.getEffectiveSampleSize() == 1.0
estimate = result.getQuantileEstimate()
assert abs(estimate - true_upper) < 1.0e-8
assert result.getCoverageType() == COVERAGE_NONE
assert abs(result.getAnalyticalProbability() - 0.05) < 1.0e-8
assert abs(result.getDesignPoint()[0] - true_upper) < 1.0e-8
assert abs(result.getReliabilityIndex() - true_upper) < 1.0e-8
assert not result.hasCurvatureCorrection()
assert not result.hasRefinedProbability()
assert result.getRefinementStandardError() == 0.0
assert result.getReplicateCount() == 0
assert result.getReplicateEstimates().getSize() == 0
assert result.getRootIterationCount() > 0
assert result.getProbabilityEvaluationCount() > result.getRootIterationCount()
assert bracket_width(result) < 1.0e-8
assert result.getRootSolverType() == BISECTION
assert result.getRootSolverDescription() == "Bisection"
assert result.getCommonRandomNumbers()
assert not result.getRandomStreamCapability()
assert not result.getDelegatedToDirectExtraction()
assert result.isFixedSample()

# ---------------------------------------------------------------------
# 3. SORM quantile on the same linear limit state
# ---------------------------------------------------------------------
algo = otexp.PostAnalyticalQuantile(output, 0.95, 0.95)
algo.setAnalyticalMethod(SORM_APPROXIMATION)
algo.setSORMFormula(HOHENBICHLER)
algo.setRootTolerance(1.0e-9)
algo.run()
result = algo.getResult()
assert abs(result.getQuantileEstimate() - true_upper) < 1.0e-8
assert abs(result.getAnalyticalProbability() - 0.05) < 1.0e-8
assert result.hasCurvatureCorrection()
assert abs(result.getCurvatureCorrection() - 1.0) < 1.0e-8
assert result.getCoverageType() == COVERAGE_NONE

# ---------------------------------------------------------------------
# 4. Multidimensional linear limit state and lower tail
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

algo = otexp.PostAnalyticalQuantile(composite_output, 0.95, 0.95)
algo.setRootTolerance(1.0e-9)
algo.run()
result = algo.getResult()
assert abs(result.getQuantileEstimate() - true_sum_quantile) < 1.0e-6
assert result.getDesignPoint().getSize() == dimension
assert abs(result.getReliabilityIndex() - true_upper) < 1.0e-8
for value in point_values(result.getDesignPoint()):
    ott.assert_almost_equal(
        value, true_upper / math.sqrt(dimension), 1.0e-6, 1.0e-6
    )
assert result.getCoverageType() == COVERAGE_NONE

algo = otexp.PostAnalyticalQuantile(output, 0.05, 0.95)
algo.setRootTolerance(1.0e-9)
algo.run()
result = algo.getResult()
assert abs(result.getQuantileEstimate() - true_lower) < 1.0e-8
assert abs(result.getAnalyticalProbability() - 0.95) < 1.0e-8
assert result.getCoverageType() == COVERAGE_NONE

# ---------------------------------------------------------------------
# 5. Importance-sampling refinement with independent replicates
# ---------------------------------------------------------------------
ot.RandomGenerator.SetSeed(20260416)
replicate_count = 10
algo = otexp.PostAnalyticalQuantile(composite_output, 0.95, 0.95)
algo.setRefinementEnabled(True)
algo.setReplicateCount(replicate_count)
algo.setSampleSize(2000)
algo.setRootTolerance(1.0e-6)
algo.run()
refined = algo.getResult()

assert refined.getSampleSize() == algo.getPilotSampleSize() + replicate_count * 2000
assert refined.getEffectiveSampleSize() == replicate_count * 2000
assert refined.getCoverageType() == COVERAGE_ASYMPTOTIC
assert refined.hasRefinedProbability()
assert abs(refined.getRefinedProbability() - 0.05) < 1.0e-3
assert refined.hasAnalyticalProbability()
assert abs(refined.getAnalyticalProbability() - 0.05) < 1.0e-3
assert refined.hasDesignPoint()
assert refined.hasReliabilityIndex()
assert refined.getReplicateCount() == replicate_count
replicates = point_values(refined.getReplicateEstimates())
assert len(replicates) == replicate_count
mean = sum(replicates) / replicate_count
variance = sum((value - mean) ** 2 for value in replicates) / (
    replicate_count - 1
)
standard_error = math.sqrt(variance / replicate_count)
ott.assert_almost_equal(
    refined.getQuantileEstimate(), mean, 1.0e-12, 1.0e-12
)
ott.assert_almost_equal(
    refined.getStandardError(), standard_error, 1.0e-12, 1.0e-12
)
ott.assert_almost_equal(
    refined.getRefinementStandardError(), standard_error, 1.0e-12, 1.0e-12
)
t = ot.DistFunc.qStudent(replicate_count - 1, 0.975)
interval = refined.getConfidenceInterval()
ott.assert_almost_equal(
    interval.getLowerBound()[0], mean - t * standard_error, 1.0e-12, 1.0e-12
)
ott.assert_almost_equal(
    interval.getUpperBound()[0], mean + t * standard_error, 1.0e-12, 1.0e-12
)
assert interval.getLowerBound()[0] <= true_sum_quantile <= interval.getUpperBound()[0]

# ---------------------------------------------------------------------
# 6. Persistence and copy
# ---------------------------------------------------------------------
ot.RandomGenerator.SetSeed(20260417)
algo = otexp.PostAnalyticalQuantile(output, 0.95, 0.95)
algo.setAnalyticalMethod(SORM_APPROXIMATION)
algo.setSORMFormula(TVEDT)
algo.setRefinementEnabled(True)
algo.setReplicateCount(5)
algo.setSampleSize(500)
algo.setRootTolerance(1.0e-6)
algo.run()
result = algo.getResult()

path = "post_analytical_quantile.xml"
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager(path))
study.add("result", result)
study.add("algo", algo)
study.save()

study = ot.Study()
study.setStorageManager(ot.XMLStorageManager(path))
study.load()
loaded_algo = otexp.PostAnalyticalQuantile()
study.fillObject("algo", loaded_algo)
loaded_result = otexp.QuantileSimulationResult()
study.fillObject("result", loaded_result)
os.remove(path)

assert loaded_algo.getAnalyticalMethod() == SORM_APPROXIMATION
assert loaded_algo.getSORMFormula() == TVEDT
assert loaded_algo.getRefinementEnabled()
assert loaded_algo.getReplicateCount() == 5
assert loaded_algo.getSampleSize() == 500
assert loaded_result.getQuantileEstimate() == result.getQuantileEstimate()
assert loaded_result.getCoverageType() == result.getCoverageType()
assert (
    loaded_result.getAnalyticalProbability()
    == result.getAnalyticalProbability()
)
assert loaded_result.getDesignPoint() == result.getDesignPoint()
assert loaded_result.getReliabilityIndex() == result.getReliabilityIndex()
assert (
    loaded_result.hasCurvatureCorrection()
    == result.hasCurvatureCorrection()
)
assert (
    loaded_result.getCurvatureCorrection()
    == result.getCurvatureCorrection()
)
assert loaded_result.hasRefinedProbability()
assert (
    loaded_result.getRefinedProbability()
    == result.getRefinedProbability()
)
assert (
    loaded_result.getRefinementStandardError()
    == result.getRefinementStandardError()
)
assert loaded_result.getReplicateCount() == result.getReplicateCount()
assert (
    loaded_result.getReplicateEstimates()
    == result.getReplicateEstimates()
)
assert loaded_result.getRootBracket() == result.getRootBracket()

copied = otexp.PostAnalyticalQuantile(algo)
assert copied.getAnalyticalMethod() == SORM_APPROXIMATION
assert copied.getSORMFormula() == TVEDT
assert copied.getReplicateCount() == 5

# ---------------------------------------------------------------------
# 7. Invalid arguments
# ---------------------------------------------------------------------
with ott.assert_raises(TypeError):
    otexp.PostAnalyticalQuantile(output, 0.0, 0.95)
with ott.assert_raises(TypeError):
    otexp.PostAnalyticalQuantile(output, 1.0, 0.95)
with ott.assert_raises(TypeError):
    otexp.PostAnalyticalQuantile(output, 0.95, 0.0)
with ott.assert_raises(TypeError):
    otexp.PostAnalyticalQuantile(output, 0.95, 1.0)
with ott.assert_raises(TypeError):
    otexp.PostAnalyticalQuantile(ot.RandomVector(ot.Normal(2)), 0.95, 0.95)

algo = otexp.PostAnalyticalQuantile(output)
with ott.assert_raises(TypeError):
    algo.setAnalyticalMethod(42)
with ott.assert_raises(TypeError):
    algo.setSORMFormula(42)
with ott.assert_raises(TypeError):
    algo.setReplicateCount(0)
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

# The default constructor inherits the Uniform() output of the base
# class, so the algorithm is runnable without any further setup.
algo = otexp.PostAnalyticalQuantile()
algo.setRootTolerance(1.0e-6)
algo.run()
assert math.isfinite(algo.getResult().getQuantileEstimate())

# The refinement needs at least two replicates for a Student interval.
algo = otexp.PostAnalyticalQuantile(output)
algo.setRefinementEnabled(True)
algo.setReplicateCount(1)
with ott.assert_raises(TypeError):
    algo.run()

algo = otexp.PostAnalyticalQuantile(output)
algo.setRefinementEnabled(True)
algo.setSampleSize(1)
with ott.assert_raises(TypeError):
    algo.run()

print("OK")

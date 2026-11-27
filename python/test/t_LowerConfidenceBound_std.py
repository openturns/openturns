#!/usr/bin/env python

import os
import tempfile

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.RandomGenerator.SetSeed(0)

# A small one-dimensional problem with its Gaussian process: small enough that
# every expected value below can be recomputed analytically. The covariance
# parameters are kept at their initial value: fitting them on three points of a
# deterministic function drives the correlation length to its lower bound, and
# the predictive deviation then becomes almost constant everywhere, which
# would hide the behavior the assertions below check.
dim = 1
model = ot.SymbolicFunction(["x"], ["(x - 0.3)^2"])
bounds = ot.Interval([0.0], [1.0])
problem = ot.OptimizationProblem()
problem.setObjective(model)
problem.setBounds(bounds)
inputSample = ot.Sample([[0.0], [0.5], [1.0]])
outputSample = model(inputSample)
covarianceModel = ot.SquaredExponential([0.3], [1.0])
basis = ot.ConstantBasisFactory(dim).build()
fitter = ot.GaussianProcessFitter(inputSample, outputSample, covarianceModel, basis)
fitter.setOptimizeParameters(False)
fitter.run()
gpr = ot.GaussianProcessRegression(fitter.getResult())
gpr.run()
gprResult = gpr.getResult()
covariance = ot.GaussianProcessConditionalCovariance(gprResult)

# ---------------------------------------------------------------------------
# Analytic: the algorithm maximizes, so a minimization uses the negated bound
# -m(x) + k * s(x); the default conservative factor comes from the ResourceMap
# ---------------------------------------------------------------------------
defaultFactor = ot.ResourceMap.GetAsScalar(
    "LowerConfidenceBound-DefaultConservativeFactor"
)
assert defaultFactor == 1.0
criterion = otexp.LowerConfidenceBound()
assert criterion.getConservativeFactor() == defaultFactor
assert criterion.getMinimization() is True
assert criterion.hasNoiseFunction() is False
assert "LowerConfidenceBound" in repr(criterion)
criterion.setGaussianProcessRegression(gprResult)
criterion.setInputSample(inputSample)
criterion.setOptimalValue(outputSample.getMin()[0])
criterion.setMinimization(True)
objective = criterion.buildObjective()
assert objective.getInputDimension() == 1
assert objective.getOutputDimension() == 1
points = [[0.1], [0.3], [0.7], [0.95]]
for point in points:
    mean = covariance.getConditionalMean(point)[0]
    variance = covariance.getConditionalMarginalVariance(point)
    deviation = variance ** 0.5 if variance > 0.0 else 0.0
    expected = defaultFactor * deviation - mean
    ott.assert_almost_equal(objective(point), [expected], 1e-12, 1e-12)
# the bound uses no reference value, so the recorded value lives on the scale
# of the objective, not of the improvement
assert objective([0.0])[0] < 0.0, objective([0.0])
# the surrogate interpolates exactly at the design points where the predictive
# variance vanishes, and the bound is then the measured value itself
for designPoint in [[0.0], [0.5], [1.0]]:
    mean = covariance.getConditionalMean(designPoint)[0]
    ott.assert_almost_equal(objective(designPoint), [-mean], 1e-12, 1e-12)

# ---------------------------------------------------------------------------
# The conservative factor scales the deviation term only: the difference of
# two objectives is exactly (k2 - k1) * s(x)
# ---------------------------------------------------------------------------
criterion.setConservativeFactor(2.0)
assert criterion.getConservativeFactor() == 2.0
assert "conservativeFactor=2" in repr(criterion)
scaledObjective = criterion.buildObjective()
for point in points:
    mean = covariance.getConditionalMean(point)[0]
    variance = covariance.getConditionalMarginalVariance(point)
    deviation = variance ** 0.5 if variance > 0.0 else 0.0
    expected = 2.0 * deviation - mean
    ott.assert_almost_equal(scaledObjective(point), [expected], 1e-12, 1e-12)
    diff = scaledObjective(point)[0] - objective(point)[0]
    ott.assert_almost_equal(diff, deviation, 1e-12, 1e-12)

# ---------------------------------------------------------------------------
# Maximization: the mirror of the bound, m(x) + k * s(x), is maximized
# ---------------------------------------------------------------------------
maxCriterion = otexp.LowerConfidenceBound(1.0)
maxCriterion.setGaussianProcessRegression(gprResult)
maxCriterion.setInputSample(inputSample)
maxCriterion.setOptimalValue(outputSample.getMax()[0])
maxCriterion.setMinimization(False)
maxObjective = maxCriterion.buildObjective()
for point in [[0.2], [0.6]]:
    mean = covariance.getConditionalMean(point)[0]
    variance = covariance.getConditionalMarginalVariance(point)
    deviation = variance ** 0.5 if variance > 0.0 else 0.0
    expected = mean + 1.0 * deviation
    ott.assert_almost_equal(maxObjective(point), [expected], 1e-12, 1e-12)

# ---------------------------------------------------------------------------
# The conservative factor is a declared parameter, not a derived quantity: it
# must be a bound on the deviation, so a negative or non-finite value is
# refused
# ---------------------------------------------------------------------------
for bad in [-1.0, float("nan"), float("inf")]:
    try:
        otexp.LowerConfidenceBound(bad)
    except Exception as exc:
        assert "nonnegative" in str(exc), (bad, exc)
    else:
        raise RuntimeError("a non-negative finite factor must be refused: %s" % bad)
refused = otexp.LowerConfidenceBound(1.0)
try:
    refused.setConservativeFactor(-0.5)
except Exception as exc:
    assert "nonnegative" in str(exc), exc
else:
    raise RuntimeError("a negative factor must be refused")

# ---------------------------------------------------------------------------
# The criterion is defined for a noiseless objective, and refuses a noise
# function: with a noise model it is not established whether the bound should
# bound the observed values or the true objective
# ---------------------------------------------------------------------------
noiseFunction = ot.SymbolicFunction(["x"], ["0.04"])
noisyCriterion = otexp.LowerConfidenceBound()
noisyCriterion.setGaussianProcessRegression(gprResult)
noisyCriterion.setInputSample(inputSample)
noisyCriterion.setOptimalValue(0.0)
noisyCriterion.setNoiseFunction(noiseFunction)
assert noisyCriterion.hasNoiseFunction() is True
try:
    noisyCriterion.buildObjective()
except Exception as exc:
    assert "noisy" in str(exc), exc
else:
    raise RuntimeError("a noisy lower confidence bound must be refused")

# ---------------------------------------------------------------------------
# Input validation, the same shape as the expected improvement's
# ---------------------------------------------------------------------------
emptyCriterion = otexp.LowerConfidenceBound()
try:
    emptyCriterion.buildObjective()
except Exception as exc:
    assert "Gaussian process" in str(exc), exc
else:
    raise RuntimeError("a lower confidence bound without surrogate must be refused")

badCriterion = otexp.LowerConfidenceBound()
badCriterion.setGaussianProcessRegression(gprResult)
badCriterion.setInputSample(ot.Sample([[0.0], [0.5], [1.0], [1.5]]))
badCriterion.setOptimalValue(0.0)
badCriterion.setMinimization(True)
try:
    badCriterion.buildObjective()
except Exception as exc:
    assert "design size" in str(exc), exc
else:
    raise RuntimeError("a design disagreeing with the surrogate must be refused")

# ---------------------------------------------------------------------------
# The handle copies on write: a copy is not a view of the original
# ---------------------------------------------------------------------------
original = otexp.LowerConfidenceBound(1.5)
criterionCopy = otexp.EfficientGlobalOptimizationCriterion(original)
criterionCopy.setOptimalValue(3.0)
assert original.getOptimalValue() == 0.0
assert criterionCopy.getOptimalValue() == 3.0
assert "LowerConfidenceBound" in repr(criterionCopy)

# ---------------------------------------------------------------------------
# Algorithm level: the criterion is a parameter of EGO, its values are recorded
# on the scale of the objective, and the AEI tradeoff accessors refuse it
# ---------------------------------------------------------------------------
algo = otexp.EfficientGlobalOptimization(problem, gprResult)
algo.setMaximumCallsNumber(2)
# The default fitted correlation length of this three-point design sits at its
# lower bound, so the historical minimum-distance rule would stop the loop
# before the second enrichment; defusing it makes the iteration count of the
# assertions below deterministic.
algo.setCorrelationLengthFactor(1e6)
assert "ExpectedImprovement" in str(algo.getCriterion())
algo.setCriterion(
    otexp.EfficientGlobalOptimizationCriterion(otexp.LowerConfidenceBound(2.0))
)
assert "LowerConfidenceBound" in repr(algo.getCriterion())
assert "conservativeFactor=2" in repr(algo.getCriterion())
try:
    algo.getAEITradeoff()
except Exception as exc:
    assert "expected improvement" in str(exc), exc
else:
    raise RuntimeError("getAEITradeoff must refuse a lower confidence bound")
algo.run()
assert algo.getResult().getIterationNumber() == 2, algo.getResult().getIterationNumber()
values = algo.getCriterionValues()
assert values.getSize() == 2, values
assert values.getMax()[0] - values.getMin()[0] < 1e6, values
assert algo.getHasConverged() is False

# ---------------------------------------------------------------------------
# Persistence, when the build supports it: the conservative factor is part of
# the criterion, not of the algorithm
# ---------------------------------------------------------------------------
if ot.PlatformInfo.HasFeature("libxml2"):
    with tempfile.TemporaryDirectory() as tmpdir:
        path = os.path.join(tmpdir, "study.xml")
        study = ot.Study()
        study.setStorageManager(ot.XMLStorageManager(path))
        study.add("algo", algo)
        study.save()
        study2 = ot.Study()
        study2.setStorageManager(ot.XMLStorageManager(path))
        study2.load()
        reloaded = otexp.EfficientGlobalOptimization()
        study2.fillObject("algo", reloaded)
        assert reloaded.getMaximumCallsNumber() == 2
        assert "LowerConfidenceBound" in repr(reloaded.getCriterion())
        assert "conservativeFactor=2" in repr(reloaded.getCriterion())
        # the criterion must still build an objective after the round trip
        reloadedCriterion = reloaded.getCriterion()
        reloadedCriterion.setGaussianProcessRegression(gprResult)
        reloadedCriterion.setInputSample(inputSample)
        reloadedCriterion.setOptimalValue(0.0)
        reloadedCriterion.setMinimization(True)
        assert reloadedCriterion.buildObjective().getOutputDimension() == 1

print("OK")

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
# Analytic: PI(x) = Phi((y_min - m(x)) / s(x)), zero where the predictive
# variance is exactly zero
# ---------------------------------------------------------------------------
criterion = otexp.ProbabilityImprovement()
assert criterion.getMinimization() is True
assert criterion.hasNoiseFunction() is False
assert "ProbabilityImprovement" in repr(criterion)
criterion.setGaussianProcessRegression(gprResult)
criterion.setInputSample(inputSample)
criterion.setOptimalValue(outputSample.getMin()[0])
criterion.setMinimization(True)
objective = criterion.buildObjective()
assert objective.getInputDimension() == 1
assert objective.getOutputDimension() == 1
best = outputSample.getMin()[0]
for point in [[0.1], [0.3], [0.7], [0.95]]:
    mean = covariance.getConditionalMean(point)[0]
    deviation = covariance.getConditionalMarginalVariance(point) ** 0.5
    expected = ot.DistFunc.pNormal((best - mean) / deviation)
    ott.assert_almost_equal(objective(point), [expected], 1e-12, 1e-12)
# the surrogate interpolates exactly at the design points where the predictive
# variance is zero, and the objective is zero there, as for the expected
# improvement
for designPoint in [[0.0], [1.0]]:
    assert covariance.getConditionalMarginalVariance(designPoint) <= 0.0
    assert objective(designPoint) == [0.0], (designPoint, objective(designPoint))

# ---------------------------------------------------------------------------
# The objective is a probability: it stays in [0, 1] over the whole domain,
# which is the saturation of the criterion at work
# ---------------------------------------------------------------------------
grid = ot.Sample([[0.01 * i] for i in range(1, 100)])
values = objective(grid)
assert values.getMin()[0] >= 0.0, values
assert values.getMax()[0] <= 1.0, values

# ---------------------------------------------------------------------------
# Maximization: the same improvement margin, mirrored
# ---------------------------------------------------------------------------
maxCriterion = otexp.ProbabilityImprovement()
maxCriterion.setGaussianProcessRegression(gprResult)
maxCriterion.setInputSample(inputSample)
maxCriterion.setOptimalValue(outputSample.getMax()[0])
maxCriterion.setMinimization(False)
maxObjective = maxCriterion.buildObjective()
bestMax = outputSample.getMax()[0]
for point in [[0.2], [0.6]]:
    mean = covariance.getConditionalMean(point)[0]
    deviation = covariance.getConditionalMarginalVariance(point) ** 0.5
    expected = ot.DistFunc.pNormal((mean - bestMax) / deviation)
    ott.assert_almost_equal(maxObjective(point), [expected], 1e-12, 1e-12)

# ---------------------------------------------------------------------------
# The criterion is defined for a noiseless objective: the probability is
# computed against the best value *observed* on the design, which a noise
# model does not provide, so a noise function is refused
# ---------------------------------------------------------------------------
noiseFunction = ot.SymbolicFunction(["x"], ["0.04"])
noisyCriterion = otexp.ProbabilityImprovement()
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
    raise RuntimeError("a noisy probability of improvement must be refused")

# ---------------------------------------------------------------------------
# Input validation, the same shape as the expected improvement's
# ---------------------------------------------------------------------------
emptyCriterion = otexp.ProbabilityImprovement()
try:
    emptyCriterion.buildObjective()
except Exception as exc:
    assert "Gaussian process" in str(exc), exc
else:
    raise RuntimeError("a probability of improvement without surrogate must be refused")

badCriterion = otexp.ProbabilityImprovement()
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
original = otexp.ProbabilityImprovement()
criterionCopy = otexp.EfficientGlobalOptimizationCriterion(original)
criterionCopy.setOptimalValue(3.0)
assert original.getOptimalValue() == 0.0
assert criterionCopy.getOptimalValue() == 3.0
assert "ProbabilityImprovement" in repr(criterionCopy)

# ---------------------------------------------------------------------------
# Algorithm level: the criterion is a parameter of EGO, its values are
# recorded and bounded as probabilities, and the AEI tradeoff accessors
# refuse it
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
    otexp.EfficientGlobalOptimizationCriterion(otexp.ProbabilityImprovement())
)
assert "ProbabilityImprovement" in repr(algo.getCriterion())
try:
    algo.getAEITradeoff()
except Exception as exc:
    assert "expected improvement" in str(exc), exc
else:
    raise RuntimeError("getAEITradeoff must refuse a probability of improvement")
algo.run()
assert algo.getResult().getIterationNumber() == 2, algo.getResult().getIterationNumber()
values = algo.getCriterionValues()
assert values.getSize() == 2, values
# the recorded values are probabilities
assert values.getMin()[0] >= 0.0, values
assert values.getMax()[0] <= 1.0, values
assert algo.getHasConverged() is False

# ---------------------------------------------------------------------------
# Persistence, when the build supports it
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
        assert "ProbabilityImprovement" in repr(reloaded.getCriterion())
        # the criterion must still build an objective after the round trip
        reloadedCriterion = reloaded.getCriterion()
        reloadedCriterion.setGaussianProcessRegression(gprResult)
        reloadedCriterion.setInputSample(inputSample)
        reloadedCriterion.setOptimalValue(0.0)
        reloadedCriterion.setMinimization(True)
        assert reloadedCriterion.buildObjective().getOutputDimension() == 1

print("OK")

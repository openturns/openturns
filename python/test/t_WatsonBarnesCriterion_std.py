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


def expectedImprovement(delta, deviation):
    """Analytic expected improvement: the first moment of the positive part of
    a normal of mean ``delta`` (the signed improvement margin, measured in the
    "smaller is better" scale of the criterion) and standard deviation
    ``deviation``."""
    if deviation <= 0.0:
        return 0.0
    ratio = delta / deviation
    return delta * ot.DistFunc.pNormal(ratio) + deviation * ot.DistFunc.dNormal(ratio)


# ---------------------------------------------------------------------------
# Analytic: WB2(x) = -m(x) + EI(x) for a minimization, where EI is the expected
# improvement over the best value observed on the design
# ---------------------------------------------------------------------------
criterion = otexp.WatsonBarnesCriterion()
assert criterion.getMinimization() is True
assert criterion.hasNoiseFunction() is False
assert "WatsonBarnesCriterion" in repr(criterion)
criterion.setGaussianProcessRegression(gprResult)
criterion.setInputSample(inputSample)
criterion.setOptimalValue(outputSample.getMin()[0])
criterion.setMinimization(True)
objective = criterion.buildObjective()
assert objective.getInputDimension() == 1
assert objective.getOutputDimension() == 1
best = outputSample.getMin()[0]
points = [[0.1], [0.3], [0.7], [0.95]]
for point in points:
    mean = covariance.getConditionalMean(point)[0]
    variance = covariance.getConditionalMarginalVariance(point)
    deviation = variance ** 0.5 if variance > 0.0 else 0.0
    expected = -mean + expectedImprovement(best - mean, deviation)
    ott.assert_almost_equal(objective(point), [expected], 1e-12, 1e-12)
# at the measured points, where the predictive variance vanishes, the expected
# improvement is zero and the criterion is the exploitation term alone: -m(x)
for designPoint in [[0.0], [0.5], [1.0]]:
    mean = covariance.getConditionalMean(designPoint)[0]
    ott.assert_almost_equal(objective(designPoint), [-mean], 1e-12, 1e-12)

# ---------------------------------------------------------------------------
# WB2 is not an improvement criterion: its value carries the scale of the
# objective, so a point far from the optimum can give a negative value while
# the expected improvement of the same point is nonnegative
# ---------------------------------------------------------------------------
worst = [0.99]
meanWorst = covariance.getConditionalMean(worst)[0]
deviationWorst = covariance.getConditionalMarginalVariance(worst) ** 0.5
assert expectedImprovement(best - meanWorst, deviationWorst) >= 0.0
assert objective(worst)[0] < 0.0, (objective(worst), meanWorst)

# ---------------------------------------------------------------------------
# Maximization: the mirror WB2(x) = m(x) + EI(x) with the margin measured
# above the best value observed on the design
# ---------------------------------------------------------------------------
maxCriterion = otexp.WatsonBarnesCriterion()
maxCriterion.setGaussianProcessRegression(gprResult)
maxCriterion.setInputSample(inputSample)
maxCriterion.setOptimalValue(outputSample.getMax()[0])
maxCriterion.setMinimization(False)
maxObjective = maxCriterion.buildObjective()
bestMax = outputSample.getMax()[0]
for point in [[0.2], [0.6]]:
    mean = covariance.getConditionalMean(point)[0]
    variance = covariance.getConditionalMarginalVariance(point)
    deviation = variance ** 0.5 if variance > 0.0 else 0.0
    expected = mean + expectedImprovement(mean - bestMax, deviation)
    ott.assert_almost_equal(maxObjective(point), [expected], 1e-12, 1e-12)

# ---------------------------------------------------------------------------
# The expected improvement term of WB2 is the one of the expected improvement
# criterion, evaluated at the same point and on the same reference level: the
# two criteria only differ by their exploitation term, so their difference is
# exactly the conditional mean of the surrogate
# ---------------------------------------------------------------------------
eiCriterion = otexp.ExpectedImprovement()
eiCriterion.setGaussianProcessRegression(gprResult)
eiCriterion.setInputSample(inputSample)
eiCriterion.setOptimalValue(best)
eiCriterion.setMinimization(True)
eiObjective = eiCriterion.buildObjective()
for point in points:
    mean = covariance.getConditionalMean(point)[0]
    diff = [objective(point)[0] - eiObjective(point)[0]]
    ott.assert_almost_equal(diff, [-mean], 1e-12, 1e-12)

# ---------------------------------------------------------------------------
# The criterion is defined for a noiseless objective, like the probability of
# improvement and the lower confidence bound: a noise function is refused
# rather than combined with an invented discount of the exploitation term
# ---------------------------------------------------------------------------
noiseFunction = ot.SymbolicFunction(["x"], ["0.04"])
noisyCriterion = otexp.WatsonBarnesCriterion()
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
    raise RuntimeError("a noisy Watson and Barnes criterion must be refused")

# ---------------------------------------------------------------------------
# Input validation, the same shape as the other criteria's
# ---------------------------------------------------------------------------
emptyCriterion = otexp.WatsonBarnesCriterion()
try:
    emptyCriterion.buildObjective()
except Exception as exc:
    assert "Gaussian process" in str(exc), exc
else:
    raise RuntimeError("a Watson and Barnes criterion without surrogate must be refused")

badCriterion = otexp.WatsonBarnesCriterion()
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
original = otexp.WatsonBarnesCriterion()
criterionCopy = otexp.EfficientGlobalOptimizationCriterion(original)
criterionCopy.setOptimalValue(3.0)
assert original.getOptimalValue() == 0.0
assert criterionCopy.getOptimalValue() == 3.0
assert "WatsonBarnesCriterion" in repr(criterionCopy)

# ---------------------------------------------------------------------------
# Algorithm level: the criterion is a parameter of EGO, its values are recorded
# on the scale of the objective (not bounded as probabilities are), and the AEI
# tradeoff accessors refuse it
# ---------------------------------------------------------------------------
algo = otexp.EfficientGlobalOptimization(problem, gprResult)
algo.setMaximumCallsNumber(2)
# The default fitted correlation length of this three-point design sits at its
# lower bound, so the historical minimum-distance rule would stop the loop
# before the second enrichment; defusing it makes the iteration count of the
# assertions below deterministic.
algo.setCorrelationLengthFactor(1e6)
assert "ExpectedImprovement" in str(algo.getCriterion())
algo.setCriterion(otexp.EfficientGlobalOptimizationCriterion(otexp.WatsonBarnesCriterion()))
assert "WatsonBarnesCriterion" in repr(algo.getCriterion())
try:
    algo.getAEITradeoff()
except Exception as exc:
    assert "expected improvement" in str(exc), exc
else:
    raise RuntimeError("getAEITradeoff must refuse a Watson and Barnes criterion")
algo.run()
assert algo.getResult().getIterationNumber() == 2, algo.getResult().getIterationNumber()
values = algo.getCriterionValues()
assert values.getSize() == 2, values
assert values.getMax()[0] - values.getMin()[0] < 1e6, values
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
        assert "WatsonBarnesCriterion" in repr(reloaded.getCriterion())
        # the criterion must still build an objective after the round trip
        reloadedCriterion = reloaded.getCriterion()
        reloadedCriterion.setGaussianProcessRegression(gprResult)
        reloadedCriterion.setInputSample(inputSample)
        reloadedCriterion.setOptimalValue(0.0)
        reloadedCriterion.setMinimization(True)
        assert reloadedCriterion.buildObjective().getOutputDimension() == 1

print("OK")

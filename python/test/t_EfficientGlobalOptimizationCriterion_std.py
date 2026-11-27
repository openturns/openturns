#!/usr/bin/env python

import os
import tempfile

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.RandomGenerator.SetSeed(0)

# A small one-dimensional problem with its Gaussian process: small enough that
# every expected value below can be recomputed analytically.
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
fitter.run()
gpr = ot.GaussianProcessRegression(fitter.getResult())
gpr.run()
gprResult = gpr.getResult()


# ---------------------------------------------------------------------------
# The base class refuses to build an objective, with a precise message
# ---------------------------------------------------------------------------
baseCriterion = otexp.EfficientGlobalOptimizationCriterionImplementation()
try:
    baseCriterion.buildObjective()
except Exception as exc:
    assert "buildObjective" in str(exc), exc
else:
    raise RuntimeError("the base criterion must not build an objective")
assert baseCriterion.checkConvergence(1.0) is False
assert baseCriterion.getMinimization() is True
assert baseCriterion.hasNoiseFunction() is False


# ---------------------------------------------------------------------------
# Expected improvement: the noiseless reference is the best observed value, and
# the objective reproduces the analytic expected improvement
# ---------------------------------------------------------------------------
criterion = otexp.ExpectedImprovement()
assert criterion.getAEITradeoff() == ot.ResourceMap.GetAsScalar(
    "EfficientGlobalOptimization-DefaultAEITradeoff"
)
criterion.setGaussianProcessRegression(gprResult)
criterion.setInputSample(inputSample)
criterion.setOptimalValue(outputSample.getMin()[0])
criterion.setMinimization(True)
objective = criterion.buildObjective()
assert objective.getInputDimension() == 1
assert objective.getOutputDimension() == 1

covariance = ot.GaussianProcessConditionalCovariance(gprResult)
best = outputSample.getMin()[0]
for point in [[0.1], [0.3], [0.7], [0.95]]:
    mean = covariance.getConditionalMean(point)[0]
    variance = covariance.getConditionalMarginalVariance(point)
    deviation = variance ** 0.5
    ratio = (best - mean) / deviation
    expected = (best - mean) * ot.DistFunc.pNormal(ratio) + deviation * ot.DistFunc.dNormal(
        ratio
    )
    ott.assert_almost_equal(objective(point), [expected], 1e-12, 1e-12)


# ---------------------------------------------------------------------------
# Maximization: the same improvement, mirrored
# ---------------------------------------------------------------------------
maxCriterion = otexp.ExpectedImprovement()
maxCriterion.setGaussianProcessRegression(gprResult)
maxCriterion.setInputSample(inputSample)
maxCriterion.setOptimalValue(outputSample.getMax()[0])
maxCriterion.setMinimization(False)
maxObjective = maxCriterion.buildObjective()
bestMax = outputSample.getMax()[0]
for point in [[0.2], [0.6]]:
    mean = covariance.getConditionalMean(point)[0]
    deviation = covariance.getConditionalMarginalVariance(point) ** 0.5
    ratio = (mean - bestMax) / deviation
    expected = (mean - bestMax) * ot.DistFunc.pNormal(ratio) + deviation * ot.DistFunc.dNormal(
        ratio
    )
    ott.assert_almost_equal(maxObjective(point), [expected], 1e-12, 1e-12)


# ---------------------------------------------------------------------------
# Noisy case: the reference level is the quantile of the *given* Gaussian
# process, and the criterion is the augmented expected improvement. Feeding a
# second, different surrogate must change the objective: a stale surrogate here
# is the defect this test protects against.
# ---------------------------------------------------------------------------
noiseFunction = ot.SymbolicFunction(["x"], ["0.04"])
noisyCriterion = otexp.ExpectedImprovement(0.5)
noisyCriterion.setInputSample(inputSample)
noisyCriterion.setOptimalValue(0.0)
noisyCriterion.setMinimization(True)
noisyCriterion.setNoiseFunction(noiseFunction)
noisyCriterion.setGaussianProcessRegression(gprResult)
noisyObjective = noisyCriterion.buildObjective()

quantile = float("inf")
for designPoint in inputSample:
    mean = covariance.getConditionalMean(designPoint)[0]
    deviation = covariance.getConditionalMarginalVariance(designPoint) ** 0.5
    quantile = min(quantile, mean + 0.5 * deviation)
for point in [[0.15], [0.45], [0.85]]:
    mean = covariance.getConditionalMean(point)[0]
    variance = covariance.getConditionalMarginalVariance(point)
    deviation = variance ** 0.5
    ratio = (quantile - mean) / deviation
    improvement = (quantile - mean) * ot.DistFunc.pNormal(
        ratio
    ) + deviation * ot.DistFunc.dNormal(ratio)
    noiseVariance = noiseFunction(point)[0]
    expected = improvement * (1.0 - noiseVariance**0.5 / (noiseVariance + variance) ** 0.5)
    ott.assert_almost_equal(noisyObjective(point), [expected], 1e-10, 1e-10)

# a second surrogate on the same design gives another objective
secondInput = ot.Sample([[0.0], [0.25], [0.75], [1.0]])
secondOutput = model(secondInput)
secondFitter = ot.GaussianProcessFitter(
    secondInput, secondOutput, ot.SquaredExponential([0.2], [1.0]), basis
)
secondFitter.run()
secondGPR = ot.GaussianProcessRegression(secondFitter.getResult())
secondGPR.run()
firstValue = noisyObjective([0.15])
# the design and the surrogate are one piece of state: they move together
noisyCriterion.setInputSample(secondInput)
noisyCriterion.setGaussianProcessRegression(secondGPR.getResult())
secondValue = noisyCriterion.buildObjective()([0.15])
assert abs(firstValue[0] - secondValue[0]) > 1e-6, (firstValue, secondValue)


# ---------------------------------------------------------------------------
# Input validation
# ---------------------------------------------------------------------------
try:
    otexp.ExpectedImprovement(float("nan"))
except Exception as exc:
    assert "finite" in str(exc), exc
else:
    raise RuntimeError("a non-finite AEI tradeoff must be refused")

emptyCriterion = otexp.ExpectedImprovement()
try:
    emptyCriterion.buildObjective()
except Exception as exc:
    assert "Gaussian process" in str(exc), exc
else:
    raise RuntimeError("an expected improvement without surrogate must be refused")

badNoise = ot.SymbolicFunction(["x"], ["-1.0"])
badCriterion = otexp.ExpectedImprovement(1.0)
badCriterion.setGaussianProcessRegression(gprResult)
badCriterion.setInputSample(inputSample)
badCriterion.setOptimalValue(0.0)
badCriterion.setNoiseFunction(badNoise)
try:
    # the noise variance is read when the criterion is evaluated, not when built
    badCriterion.buildObjective()([0.1])
except Exception as exc:
    assert "nonnegative" in str(exc), exc
else:
    raise RuntimeError("a negative noise variance must be refused")


# ---------------------------------------------------------------------------
# The handle copies on write: a copy is not a view of the original
# ---------------------------------------------------------------------------
original = otexp.ExpectedImprovement(1.0)
criterionCopy = otexp.EfficientGlobalOptimizationCriterion(original)
criterionCopy.setOptimalValue(3.0)
assert original.getOptimalValue() == 0.0
assert criterionCopy.getOptimalValue() == 3.0
assert "ExpectedImprovement" in repr(original)


# ---------------------------------------------------------------------------
# Algorithm level: the criterion is a parameter, its values are recorded, and
# the EI-specific accessors still work on the default criterion
# ---------------------------------------------------------------------------
algo = otexp.EfficientGlobalOptimization(problem, gprResult)
algo.setMaximumCallsNumber(3)
# The fitted correlation length of this three-point design sits at its lower
# bound, so the historical minimum-distance rule would stop the loop after one
# enrichment. Defusing it with a large factor makes the run deterministic in
# its iteration count, which is what the assertions below need.
algo.setCorrelationLengthFactor(1e6)
assert "ExpectedImprovement" in str(algo.getCriterion())
algo.setAEITradeoff(0.25)
assert abs(algo.getAEITradeoff() - 0.25) < 1e-15
algo.run()
# the criterion value is recorded for every enrichment the algorithm performed
values = algo.getCriterionValues()
assert algo.getResult().getIterationNumber() == 3, algo.getResult().getIterationNumber()
assert values.getSize() == 3, values
assert values == algo.getExpectedImprovement()
assert algo.getHasConverged() is False

# a criterion that declares convergence ends the loop and says so


class StoppingCriterion(otexp.PythonEfficientGlobalOptimizationCriterion):
    def buildObjective(self):
        # a crude improvement criterion, enough to drive the loop
        covariance = ot.GaussianProcessConditionalCovariance(
            self.getGaussianProcessRegression()
        )
        best = self.getOptimalValue()
        return ot.PythonFunction(
            1,
            1,
            lambda x: [max(0.0, best - covariance.getConditionalMean(x)[0])],
        )

    def checkConvergence(self, criterionValue):
        return True


stoppingAlgo = otexp.EfficientGlobalOptimization(problem, gprResult)
stoppingAlgo.setMaximumCallsNumber(5)
# the hook must be the only reason the loop stops, not the distance heuristic
stoppingAlgo.setCorrelationLengthFactor(1e6)
stoppingAlgo.setCriterion(otexp.EfficientGlobalOptimizationCriterion(StoppingCriterion()))
stoppingAlgo.run()
assert stoppingAlgo.getHasConverged() is True
assert stoppingAlgo.getResult().getIterationNumber() == 1


# the EI-only accessors refuse a foreign criterion instead of lying
try:
    stoppingAlgo.getAEITradeoff()
except Exception as exc:
    assert "expected improvement" in str(exc), exc
else:
    raise RuntimeError("getAEITradeoff must refuse a non-EI criterion")

# a criterion returning a wrong-sized objective is refused before the solver runs


class WrongDimensionCriterion(otexp.PythonEfficientGlobalOptimizationCriterion):
    def buildObjective(self):
        return ot.PythonFunction(2, 1, lambda x: [0.0])


wrongAlgo = otexp.EfficientGlobalOptimization(problem, gprResult)
wrongAlgo.setMaximumCallsNumber(1)
wrongAlgo.setCriterion(otexp.EfficientGlobalOptimizationCriterion(WrongDimensionCriterion()))
try:
    wrongAlgo.run()
except Exception as exc:
    assert "input dimension" in str(exc), exc
else:
    raise RuntimeError("a criterion with a wrong input dimension must be refused")


# ---------------------------------------------------------------------------
# Persistence, when the build supports it
# ---------------------------------------------------------------------------
if ot.PlatformInfo.HasFeature("libxml2"):
    with tempfile.TemporaryDirectory() as tmpdir:
        path = os.path.join(tmpdir, "study.xml")
        study = ot.Study()
        study.setStorageManager(ot.XMLStorageManager(path))
        study.add("algo", stoppingAlgo)
        study.save()
        study2 = ot.Study()
        study2.setStorageManager(ot.XMLStorageManager(path))
        study2.load()
        reloaded = otexp.EfficientGlobalOptimization()
        study2.fillObject("algo", reloaded)
        assert reloaded.getMaximumCallsNumber() == 5
        assert reloaded.getHasConverged() is True
        # the Python criterion must still build an objective after the round trip
        reloadedCriterion = reloaded.getCriterion()
        reloadedCriterion.setGaussianProcessRegression(gprResult)
        reloadedCriterion.setInputSample(inputSample)
        reloadedCriterion.setOptimalValue(0.0)
        reloadedCriterion.setMinimization(True)
        assert reloadedCriterion.buildObjective().getOutputDimension() == 1
        # the criterion history is persisted, and belongs to the run: the
        # reloaded algorithm starts with the single value it was saved with,
        # then the second run counts its own iterations only
        assert reloaded.getCriterionValues().getSize() == 1
        reloaded.setMaximumCallsNumber(2)
        reloaded.run()
        assert reloaded.getResult().getIterationNumber() == 1
        assert reloaded.getCriterionValues().getSize() == 1

print("OK")

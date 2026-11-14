#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import os

ot.TESTPREAMBLE()
ot.PlatformInfo.SetNumericalPrecision(2)

m = 10
x = [[0.5 + i] for i in range(m)]

inVars = ["a", "b", "c", "x"]
formulas = ["a + b * exp(c * x)", "(a * x^2 + b) / (c + x^2)"]
model = ot.SymbolicFunction(inVars, formulas)
p_ref = [2.8, 1.2, 0.5]
params = [0, 1, 2]
modelX = ot.ParametricFunction(model, params, p_ref)
y = modelX(x)
y += ot.Normal([0.0] * 2, [0.05] * 2, ot.IdentityMatrix(2)).getSample(m)
candidate = [1.0] * 3
priorCovariance = ot.CovarianceMatrix(3)
for i in range(3):
    priorCovariance[i, i] = 3.0 + (1.0 + i) * (1.0 + i)
    for j in range(i):
        priorCovariance[i, j] = 1.0 / (1.0 + i + j)
errorCovariance = ot.CovarianceMatrix(2)
for i in range(2):
    errorCovariance[i, i] = 2.0 + (1.0 + i) * (1.0 + i)
    for j in range(i):
        errorCovariance[i, j] = 1.0 / (1.0 + i + j)
globalErrorCovariance = ot.CovarianceMatrix(2 * m)
for i in range(2 * m):
    globalErrorCovariance[i, i] = 2.0 + (1.0 + i) * (1.0 + i)
    for j in range(i):
        globalErrorCovariance[i, j] = 1.0 / (1.0 + i + j)
bootstrapSizes = [0, 30]
for bootstrapSize in bootstrapSizes:
    algo = ot.GaussianNonLinearCalibration(
        modelX, x, y, candidate, priorCovariance, errorCovariance
    )
    algo.setBootstrapSize(bootstrapSize)
    algo.run()
    # To avoid discrepance between the platforms with or without CMinpack
    print("result   (Auto)=", algo.getResult().getParameterMAP())
    multiStartSize = 10
    algo.setOptimizationAlgorithm(
        ot.MultiStart(
            ot.TNC(),
            ot.LowDiscrepancyExperiment(
                ot.SobolSequence(),
                ot.Normal(
                    candidate, ot.CovarianceMatrix(ot.Point(candidate).getDimension())
                ),
                multiStartSize,
            ).generate(),
        )
    )
    algo.run()
    result = algo.getResult()
    # To avoid discrepance between the platforms with or without CMinpack
    print("result    (TNC)=", result.getParameterMAP())
    print("error=", result.getObservationsError())
    algo = ot.GaussianNonLinearCalibration(
        modelX, x, y, candidate, priorCovariance, globalErrorCovariance
    )
    algo.setBootstrapSize(bootstrapSize)
    algo.run()
    result = algo.getResult()
    print("result (Global)=", result.getParameterMAP())
    # Draw result
    graph = result.drawParameterDistributions()
    graph = result.drawResiduals()
    graph = result.drawObservationsVsInputs()
    graph = result.drawObservationsVsPredictions()

# unobserved inputs
p_ref = [2.8, 1.2, 0.5, 2.0]
params = [0, 1, 2, 3]
modelX = ot.ParametricFunction(model, params, p_ref)
x = ot.Sample(m, 0)
y = modelX(x)
y += ot.Normal([0.0] * 2, [0.05] * 2, ot.IdentityMatrix(2)).getSample(m)
priorCovariance = ot.CovarianceMatrix(4)
for i in range(4):
    priorCovariance[i, i] = 3.0 + (1.0 + i) * (1.0 + i)
    for j in range(i):
        priorCovariance[i, j] = 1.0 / (1.0 + i + j)
candidate = [1.0] * 4
algo = ot.GaussianNonLinearCalibration(
    modelX, x, y, candidate, priorCovariance, errorCovariance
)
algo.run()
result = algo.getResult()
ot.PlatformInfo.SetNumericalPrecision(2)
print("result (unobs.)=", result.getParameterMAP())
print("error=", result.getObservationsError())

# test output at mean
modelX.setParameter(result.getParameterPrior().getMean())
outputAtPriorMean = modelX(x)
ott.assert_almost_equal(result.getOutputAtPriorMean(), outputAtPriorMean)

modelX.setParameter(result.getParameterPosterior().getMean())
outputAtPosteriorMean = modelX(x)
ott.assert_almost_equal(result.getOutputAtPosteriorMean(), outputAtPosteriorMean)

# test bounds
m = 10
x = [[0.5 + i] for i in range(m)]
inVars = ["a", "b", "c", "x"]
formulas = ["a + b * exp(c * x)", "(a * x^2 + b) / (c + x^2)"]
model = ot.SymbolicFunction(inVars, formulas)
p_ref = [2.8, 1.2, 0.5]
params = [0, 1, 2]
modelX = ot.ParametricFunction(model, params, p_ref)
y = modelX(x)
y += ot.Normal([0.0] * 2, [0.05] * 2, ot.IdentityMatrix(2)).getSample(m)
candidate = [1.0] * 3
priorCovariance = ot.CovarianceMatrix(3)
for i in range(3):
    priorCovariance[i, i] = 3.0 + (1.0 + i) * (1.0 + i)
    for j in range(i):
        priorCovariance[i, j] = 1.0 / (1.0 + i + j)
errorCovariance = ot.CovarianceMatrix(2)
for i in range(2):
    errorCovariance[i, i] = 2.0 + (1.0 + i) * (1.0 + i)
    for j in range(i):
        errorCovariance[i, j] = 1.0 / (1.0 + i + j)
algo = ot.GaussianNonLinearCalibration(
    modelX, x, y, candidate, priorCovariance, errorCovariance
)
algo.setBootstrapSize(0)
bounds = ot.Interval([0.0] * 3, [2.0, 2.0, 1.0])
algo.setBounds(bounds)
ott.assert_almost_equal(algo.getBounds().getLowerBound(), bounds.getLowerBound())
ott.assert_almost_equal(algo.getBounds().getUpperBound(), bounds.getUpperBound())
algo.setOptimizationAlgorithm(ot.TNC())
algo.run()
boundedMAP = algo.getResult().getParameterMAP()
print("result (bounded)=", boundedMAP)
assert bounds.contains(boundedMAP), "MAP should respect bounds"
ott.assert_almost_equal(boundedMAP[0], 2.0, 1e-1, 1e-2)
# active-set posterior: Normal with zero variance in fixed directions
posterior = algo.getResult().getParameterPosterior()
assert posterior.getImplementation().getClassName() == "Normal"
tol = ot.ResourceMap.GetAsScalar("LinearLeastSquaresCalibration-ActiveBoundTolerance")
for i in range(3):
    active = (abs(boundedMAP[i] - bounds.getLowerBound()[i]) <= tol) or (
        abs(boundedMAP[i] - bounds.getUpperBound()[i]) <= tol
    )
    if active:
        assert posterior.getCovariance()[i, i] == 0.0
        for j in range(3):
            assert posterior.getCovariance()[i, j] == 0.0
            assert posterior.getCovariance()[j, i] == 0.0
# the first component saturates the upper bound in this setup
assert abs(boundedMAP[0] - bounds.getUpperBound()[0]) <= 1e-4
assert posterior.getCovariance()[0, 0] == 0.0

# test the truncated handling: the Gaussian surrogate at the MAP is truncated
# to the bounds, and the MAP is the mode of the resulting distribution
ot.ResourceMap.SetAsString("GaussianNonLinearCalibration-BoundsHandling", "Truncated")
algoTruncated = ot.GaussianNonLinearCalibration(
    modelX, x, y, candidate, priorCovariance, errorCovariance
)
algoTruncated.setBootstrapSize(0)
algoTruncated.setBounds(bounds)
algoTruncated.setOptimizationAlgorithm(ot.TNC())
algoTruncated.run()
truncatedResult = algoTruncated.getResult()
truncatedMAP = truncatedResult.getParameterMAP()
ott.assert_almost_equal(truncatedMAP, boundedMAP)
truncatedPosterior = truncatedResult.getParameterPosterior()
assert truncatedPosterior.getImplementation().getClassName() == "TruncatedDistribution"
assert truncatedPosterior.getImplementation().getBounds() == bounds
assert bounds.contains(truncatedMAP)
truncatedMean = truncatedPosterior.getMean()
assert bounds.contains(truncatedMean)

# For an affine model the truncated Laplace surrogate must reproduce the exact
# truncated posterior of GaussianLinearCalibration: the surrogate mean is
# shifted by -H^{-1} g, where g is the gradient of the negative log posterior
# at the constrained MAP, so that it coincides with the unconstrained posterior
# mean even when the MAP lies on a bound.
affineG = ot.SymbolicFunction(["a", "b", "x"], ["a + b * x"])
affineModel = ot.ParametricFunction(affineG, [0, 1], [2.0, 3.0])
affineX = ot.Sample([[0.0], [1.0], [2.0], [3.0], [4.0]])
affineY = affineModel(affineX)
affinePriorCovariance = ot.CovarianceMatrix(2)
affinePriorCovariance[0, 0] = 1.0
affinePriorCovariance[1, 1] = 1.0
affineErrorCovariance = ot.CovarianceMatrix(1, [1.0])
affineBounds = ot.Interval([-10.0, -10.0], [0.5, 10.0])
ot.ResourceMap.SetAsString("GaussianLinearCalibration-BoundsHandling", "Truncated")
affineGLC = ot.GaussianLinearCalibration(
    affineModel,
    affineX,
    affineY,
    [1.0, 1.0],
    affinePriorCovariance,
    affineErrorCovariance,
    "SVD",
)
affineGLC.setBounds(affineBounds)
affineGLC.run()
affineGNLC = ot.GaussianNonLinearCalibration(
    affineModel,
    affineX,
    affineY,
    [1.0, 1.0],
    affinePriorCovariance,
    affineErrorCovariance,
)
affineGNLC.setBootstrapSize(0)
affineGNLC.setBounds(affineBounds)
affineGNLC.setOptimizationAlgorithm(ot.TNC())
affineGNLC.run()
affineGLCUnderlying = (
    affineGLC.getResult().getParameterPosterior().getImplementation().getDistribution()
)
affineGNLCUnderlying = (
    affineGNLC.getResult().getParameterPosterior().getImplementation().getDistribution()
)
ott.assert_almost_equal(
    affineGNLCUnderlying.getMean(), affineGLCUnderlying.getMean(), 1e-5
)
ott.assert_almost_equal(
    affineGNLCUnderlying.getCovariance(), affineGLCUnderlying.getCovariance(), 1e-5
)
ott.assert_almost_equal(
    affineGNLC.getResult().getParameterMAP(),
    affineGLC.getResult().getParameterMAP(),
    1e-4,
)
ot.ResourceMap.SetAsString("GaussianLinearCalibration-BoundsHandling", "ActiveSet")
ot.ResourceMap.SetAsString("GaussianNonLinearCalibration-BoundsHandling", "ActiveSet")

# an unknown handling value is rejected by the ResourceMap
with ott.assert_raises(TypeError):
    ot.ResourceMap.SetAsString("GaussianNonLinearCalibration-BoundsHandling", "Bogus")

# setOptimizationAlgorithm preserves bounds carried by the solver problem
boundedProblem = ot.OptimizationProblem(
    ot.SymbolicFunction(["a", "b", "c"], ["a^2+b^2+c^2"]),
    ot.Function(),
    ot.Function(),
    bounds,
)
algo2 = ot.GaussianNonLinearCalibration(
    modelX, x, y, candidate, priorCovariance, errorCovariance
)
algo2.setOptimizationAlgorithm(ot.TNC(boundedProblem))
print("bounds (from algo)=", algo2.getBounds())
ott.assert_almost_equal(algo2.getBounds().getLowerBound(), bounds.getLowerBound())
ott.assert_almost_equal(algo2.getBounds().getUpperBound(), bounds.getUpperBound())

# wrong bounds dimension raises on setBounds
algo2.setBootstrapSize(0)
with ott.assert_raises(TypeError):
    algo2.setBounds(ot.Interval([0.0] * 2, [1.0] * 2))
print("wrong bounds dimension raises")

# setOptimizationAlgorithm with incompatible bounds leaves the object unchanged
wrongProblem = ot.OptimizationProblem(ot.SymbolicFunction(["a", "b"], ["a^2+b^2"]))
wrongProblem.setBounds(ot.Interval([0.0] * 2, [1.0] * 2))
boundsBefore = algo2.getBounds()
with ott.assert_raises(TypeError):
    algo2.setOptimizationAlgorithm(ot.TNC(wrongProblem))
ott.assert_almost_equal(algo2.getBounds().getLowerBound(), boundsBefore.getLowerBound())
ott.assert_almost_equal(algo2.getBounds().getUpperBound(), boundsBefore.getUpperBound())

# save/load preserves bounds
if ot.PlatformInfo.HasFeature("libxml2"):
    study = ot.Study()
    fileName = "testBoundsGaussianCalibration.xml"
    study.setStorageManager(ot.XMLStorageManager(fileName))
    study.add("algo", algo)
    study.save()
    loadedAlgo = ot.GaussianNonLinearCalibration()
    study2 = ot.Study()
    study2.setStorageManager(ot.XMLStorageManager(fileName))
    study2.load()
    study2.fillObject("algo", loadedAlgo)
    print("bounds (loaded)=", loadedAlgo.getBounds())
    ott.assert_almost_equal(
        loadedAlgo.getBounds().getLowerBound(), bounds.getLowerBound()
    )
    ott.assert_almost_equal(
        loadedAlgo.getBounds().getUpperBound(), bounds.getUpperBound()
    )
    os.remove(fileName)

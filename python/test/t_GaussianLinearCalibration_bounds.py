#! /usr/bin/env python

import os

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(0)

# Linear model y = a + b*x
g = ot.SymbolicFunction(["a", "b", "x"], ["a + b*x"])
model = ot.ParametricFunction(g, [0, 1], [2.0, 3.0])
x = ot.Sample([[0.0], [1.0], [2.0], [3.0], [4.0]])
y = model(x)
y += ot.Normal(0.0, 1.0).getSample(x.getSize())

parameterMean = [1.0, 1.0]
priorCovariance = ot.CovarianceMatrix(2)
priorCovariance[0, 0] = 1.0
priorCovariance[1, 1] = 1.0
errorCovariance = ot.CovarianceMatrix(1, [1.0])
methodName = "SVD"

# Without HiGHS, bounded calibration relies on the first generic algorithm
# returned by OptimizationAlgorithm::Build (Cobyla), which is less accurate
# than the HiGHS quadratic solver.
boundedMapRtol = 1e-5 if ot.PlatformInfo.HasFeature("highs") else 1e-3

# Unconstrained reference
reference = ot.GaussianLinearCalibration(
    model, x, y, parameterMean, priorCovariance, errorCovariance, methodName
)
assert reference.getBounds().getDimension() == 0
reference.run()
unboundedMAP = reference.getResult().getParameterMAP()
unboundedCovariance = reference.getResult().getParameterPosterior().getCovariance()

# A non-empty interval must have the parameter dimension
with ott.assert_raises(TypeError):
    reference.setBounds(ot.Interval([0.0], [1.0]))

# First parameter active at its upper bound
bounds = ot.Interval([-10.0, -10.0], [0.5, 10.0])
algo = ot.GaussianLinearCalibration(
    model, x, y, parameterMean, priorCovariance, errorCovariance, methodName
)
algo.setBounds(bounds)
assert algo.getBounds() == bounds
tolerance = ot.ResourceMap.GetAsScalar(
    "LinearLeastSquaresCalibration-ActiveBoundTolerance"
)
# the unconstrained optimum is outside the bounds in this setup
assert abs(unboundedMAP[0] - bounds.getUpperBound()[0]) > tolerance
algo.run()
result = algo.getResult()
activeMAP = result.getParameterMAP()
assert bounds.contains(activeMAP)
ott.assert_almost_equal(activeMAP[0], 0.5, boundedMapRtol, boundedMapRtol)

# Active-set posterior: Normal with a zero variance in the fixed direction
activePosterior = result.getParameterPosterior()
assert activePosterior.getImplementation().getClassName() == "Normal"
ott.assert_almost_equal(activePosterior.getCovariance()[0, 0], 0.0)
ott.assert_almost_equal(activePosterior.getCovariance()[0, 1], 0.0)
ott.assert_almost_equal(activePosterior.getCovariance()[1, 0], 0.0)
assert activePosterior.getCovariance()[1, 1] > 0.0

# Truncated handling: exact truncation of the unconstrained posterior
ot.ResourceMap.SetAsString("GaussianLinearCalibration-BoundsHandling", "Truncated")
truncatedAlgo = ot.GaussianLinearCalibration(
    model, x, y, parameterMean, priorCovariance, errorCovariance, methodName
)
truncatedAlgo.setBounds(bounds)
truncatedAlgo.run()
truncatedResult = truncatedAlgo.getResult()
truncatedMAP = truncatedResult.getParameterMAP()
# The QP mode is the same for both handlings
ott.assert_almost_equal(truncatedMAP, activeMAP)
truncatedPosterior = truncatedResult.getParameterPosterior()
truncatedImplementation = truncatedPosterior.getImplementation()
assert truncatedImplementation.getClassName() == "TruncatedDistribution"
assert truncatedImplementation.getBounds() == bounds
# The truncated distribution is built from the unconstrained posterior
underlying = truncatedImplementation.getDistribution()
assert underlying.getImplementation().getClassName() == "Normal"
ott.assert_almost_equal(underlying.getMean(), unboundedMAP)
ott.assert_almost_equal(underlying.getCovariance(), unboundedCovariance)
# The MAP is the mode: the mean of the truncated distribution differs from it
truncatedMean = truncatedPosterior.getMean()
assert bounds.contains(truncatedMean)
assert abs(truncatedMean[0] - truncatedMAP[0]) > 1e-3
# The truncated covariance is positive definite on both directions
assert truncatedPosterior.getCovariance()[0, 0] > 0.0
assert truncatedPosterior.getCovariance()[1, 1] > 0.0

ot.ResourceMap.SetAsString("GaussianLinearCalibration-BoundsHandling", "ActiveSet")

# An unknown handling value is rejected by the ResourceMap
with ott.assert_raises(TypeError):
    ot.ResourceMap.SetAsString("GaussianLinearCalibration-BoundsHandling", "Bogus")

# Wide bounds: the constrained solution is the unconstrained one
wideBounds = ot.Interval([-100.0, -100.0], [100.0, 100.0])
wideAlgo = ot.GaussianLinearCalibration(
    model, x, y, parameterMean, priorCovariance, errorCovariance, methodName
)
wideAlgo.setBounds(wideBounds)
wideAlgo.run()
ott.assert_almost_equal(wideAlgo.getResult().getParameterMAP(), unboundedMAP)
ott.assert_almost_equal(
    wideAlgo.getResult().getParameterPosterior().getCovariance(), unboundedCovariance
)
ot.ResourceMap.SetAsString("GaussianLinearCalibration-BoundsHandling", "Truncated")
wideTruncatedAlgo = ot.GaussianLinearCalibration(
    model, x, y, parameterMean, priorCovariance, errorCovariance, methodName
)
wideTruncatedAlgo.setBounds(wideBounds)
wideTruncatedAlgo.run()
ott.assert_almost_equal(wideTruncatedAlgo.getResult().getParameterMAP(), unboundedMAP)
ot.ResourceMap.SetAsString("GaussianLinearCalibration-BoundsHandling", "ActiveSet")

# Save/load preserves the bounds
if ot.PlatformInfo.HasFeature("libxml2"):
    study = ot.Study()
    fileName = "testGaussianLinearCalibrationBounds.xml"
    study.setStorageManager(ot.XMLStorageManager(fileName))
    study.add("algo", algo)
    study.save()
    loadedAlgo = ot.GaussianLinearCalibration()
    study2 = ot.Study()
    study2.setStorageManager(ot.XMLStorageManager(fileName))
    study2.load()
    study2.fillObject("algo", loadedAlgo)
    assert loadedAlgo.getBounds() == bounds
    os.remove(fileName)

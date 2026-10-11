#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Instantiate one distribution object
dim = 2
copula = ot.GumbelCopula(2.5)
assert copula.getClassName() == "GumbelCopula"
ott.assert_almost_equal(copula.getMean(), [0.5, 0.5], 0.0, 0.0)
ott.assert_almost_equal(
    copula.getCovariance(),
    ot.CovarianceMatrix(2, [0.0833333, 0.065655, 0.065655, 0.0833333]),
    1e-6,
    0.0,
)

# Is this copula an elliptical distribution?
assert not copula.isElliptical()

# Is this copula elliptical ?
assert not copula.hasEllipticalCopula()

# Is this copula independent ?
assert not copula.hasIndependentCopula()

# Test for realization of distribution
oneRealization = copula.getRealization()
ott.assert_almost_equal(oneRealization, [0.659445, 0.596689], 1e-4, 0.0)

# Test for sampling
size = 10
oneSample = copula.getSample(size)
ott.assert_almost_equal(
    oneSample,
    [
        [0.92546, 0.834663],
        [0.239934, 0.114143],
        [0.705071, 0.41781],
        [0.94214, 0.955906],
        [0.814987, 0.721541],
        [0.254655, 0.325386],
        [0.694987, 0.506464],
        [0.87143, 0.917787],
        [0.191102, 0.290004],
        [0.881289, 0.654839],
    ],
    1e-4,
    0.0,
)

# Test for sampling
size = 10000
anotherSample = copula.getSample(size)
ott.assert_almost_equal(anotherSample.computeMean(), [0.504187, 0.500439], 1e-4, 0.0)
ott.assert_almost_equal(
    anotherSample.computeCovariance(),
    ot.CovarianceMatrix(2, [0.0840685, 0.0659466, 0.0659466, 0.0842036]),
    1e-4,
    0.0,
)

# Define a point
point = ot.Point(dim, 0.2)

# Show PDF and CDF of point
pointPDF = copula.computePDF(point)
pointCDF = copula.computeCDF(point)
pointCDF = copula.computeCDF(point)
ott.assert_almost_equal(pointPDF, 2.220596, 1e-5, 0.0)
ott.assert_almost_equal(pointCDF, 0.11959239130331008, 1e-12, 0.0)

# Get 50% quantile
quantile = copula.computeQuantile(0.5)
ott.assert_almost_equal(quantile, [0.591374, 0.591374], 1e-6, 0.0)
ott.assert_almost_equal(copula.computeCDF(quantile), 0.5, 1e-6, 0.0)
# Get 95% survival function
inverseSurvival = ot.Point(copula.computeInverseSurvivalFunction(0.95))
ott.assert_almost_equal(inverseSurvival, [0.0298629, 0.0298629], 1e-6, 0.0)
ott.assert_almost_equal(
    copula.computeSurvivalFunction(inverseSurvival), 0.95, 1e-6, 0.0
)
ott.assert_almost_equal(copula.computeEntropy(), -0.561983, 1e-6, 0.0)

# Confidence regions
interval, threshold = copula.computeMinimumVolumeIntervalWithMarginalProbability(0.95)
ott.assert_almost_equal(interval.getLowerBound(), [0.016409256143060436, 0.016409256143060436], 1e-6, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [0.983591, 0.983591], 1e-6, 0.0)
ott.assert_almost_equal(threshold, 0.967181, 1e-6, 0.0)
levelSet, beta = copula.computeMinimumVolumeLevelSetWithThreshold(0.95)
ott.assert_almost_equal(beta, 0.402745, 1e-6, 0.0)
interval, beta = copula.computeBilateralConfidenceIntervalWithMarginalProbability(0.95)
ott.assert_almost_equal(interval.getLowerBound(), [0.016409256143060436, 0.016409256143060436], 1e-6, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [0.983591, 0.983591], 1e-6, 0.0)
ott.assert_almost_equal(beta, 0.967181, 1e-6, 0.0)
interval, beta = copula.computeUnilateralConfidenceIntervalWithMarginalProbability(
    0.95, False
)
ott.assert_almost_equal(interval.getLowerBound(), [0.0, 0.0], 0.0, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [0.961873, 0.961873], 1e-6, 0.0)
ott.assert_almost_equal(beta, 0.961873, 1e-6, 0.0)
interval, beta = copula.computeUnilateralConfidenceIntervalWithMarginalProbability(
    0.95, True
)
ott.assert_almost_equal(interval.getLowerBound(), [0.0298629, 0.0298629], 1e-6, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [1.0, 1.0], 0.0, 0.0)
ott.assert_almost_equal(beta, 0.970137, 1e-6, 0.0)

# Extract the marginals
for i in range(dim):
    margin = copula.getMarginal(i)
    assert margin.getImplementation().getClassName() == "IndependentCopula"
    ott.assert_almost_equal(margin.computePDF(ot.Point(1, 0.25)), 1.0, 0.0, 0.0)
    ott.assert_almost_equal(margin.computeCDF(ot.Point(1, 0.25)), 0.25, 0.0, 0.0)
    ott.assert_almost_equal(margin.computeQuantile(0.95), [0.95], 0.0, 0.0)

margin0Realization = copula.getMarginal(0).getRealization()
ott.assert_almost_equal(margin0Realization, [0.000332389655649301], 1e-6, 0.0)
margin1Realization = copula.getMarginal(1).getRealization()
ott.assert_almost_equal(margin1Realization, [0.910321], 1e-6, 0.0)

# Extract a 2-D marginal
indices = [1, 0]
assert indices == [1, 0]
margins = copula.getMarginal(indices)
assert margins.getImplementation().getClassName() == "GumbelCopula"
ott.assert_almost_equal(margins.computePDF(ot.Point(2, 0.25)), 2.03487126151337, 1e-6, 0.0)
ott.assert_almost_equal(margins.computeCDF(ot.Point(2, 0.25)), 0.160537715551504, 1e-6, 0.0)
quantile = ot.Point(margins.computeQuantile(0.95))
ott.assert_almost_equal(quantile, [0.961872813046878, 0.961872813046878], 1e-6, 0.0)
ott.assert_almost_equal(margins.computeCDF(quantile), 0.95, 1e-6, 0.0)
marginsRealization = margins.getRealization()
ott.assert_almost_equal(marginsRealization, [0.281456, 0.596274], 1e-5, 0.0)

ott.assert_almost_equal(
    copula.computeUpperTailDependenceMatrix(),
    ot.CorrelationMatrix(2, [1.0, 0.6804920892271058, 0.6804920892271058, 1.0]),
    1e-6,
    0.0,
)
ott.assert_almost_equal(
    copula.computeLowerTailDependenceMatrix(),
    ot.CorrelationMatrix(2, [1.0, 0.0, 0.0, 1.0]),
    0.0,
    0.0,
)

ot.Log.Show(ot.Log.TRACE)
ot.RandomGenerator.SetSeed(0)
validation = ott.DistributionValidation(copula)
validation.run()

# Verify upper tail dependence: lambda_U = 2 - 2^(1/theta)
chi = copula.computeUpperTailDependenceMatrix()[0, 1]
ott.assert_almost_equal(chi, 2.0 - 2.0 ** (1.0 / 2.5), 1e-5, 1e-5)

#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Instantiate one distribution object
dim = 2
copula = ot.FrankCopula(2.5)
assert copula.getClassName() == "FrankCopula"
ott.assert_almost_equal(copula.getMean(), [0.5, 0.5], 0.0, 0.0)
ott.assert_almost_equal(
    copula.getCovariance(),
    ot.CovarianceMatrix(2, [0.08333333333333333, 0.032135160021189636, 0.032135160021189636, 0.08333333333333333]),
    1e-12,
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
ott.assert_almost_equal(oneRealization, [0.629877, 0.895308], 1e-4, 0.0)

# Define a point
point = ot.Point(dim, 0.2)

# Show PDF and CDF of point
pointPDF = copula.computePDF(point)
pointCDF = copula.computeCDF(point)
ott.assert_almost_equal(pointPDF, 1.449735, 1e-5, 0.0)
ott.assert_almost_equal(pointCDF, 0.073888, 1e-5, 0.0)

# Get 50% quantile
quantile = copula.computeQuantile(0.5)
ott.assert_almost_equal(quantile, [0.662772, 0.662772], 1e-5, 0.0)
ott.assert_almost_equal(copula.computeCDF(quantile), 0.5, 1e-5, 0.0)

# Get 5% quantile
quantile = copula.computeQuantile(0.95, True)
ott.assert_almost_equal(quantile, [0.159247, 0.159247], 1e-5, 0.0)

# Get 95% survival function
inverseSurvival = ot.Point(copula.computeInverseSurvivalFunction(0.95))
ott.assert_almost_equal(inverseSurvival, [0.0258555, 0.0258555], 1e-5, 0.0)
ott.assert_almost_equal(
    copula.computeSurvivalFunction(inverseSurvival), 0.95, 1e-5, 0.0
)
ott.assert_almost_equal(copula.computeEntropy(), -0.079520, 1e-5, 0.0)

# Confidence regions
interval, threshold = copula.computeMinimumVolumeIntervalWithMarginalProbability(0.95)
ott.assert_almost_equal(interval.getLowerBound(), [0.0127327, 0.0127327], 1e-6, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [0.987267, 0.987267], 1e-6, 0.0)
ott.assert_almost_equal(threshold, 0.974535, 1e-6, 0.0)
levelSet, beta = copula.computeMinimumVolumeLevelSetWithThreshold(0.95)
ott.assert_almost_equal(beta, 0.515692, 1e-6, 0.0)
interval, beta = copula.computeBilateralConfidenceIntervalWithMarginalProbability(0.95)
ott.assert_almost_equal(interval.getLowerBound(), [0.0127327, 0.0127327], 1e-6, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [0.987267, 0.987267], 1e-6, 0.0)
ott.assert_almost_equal(beta, 0.974535, 1e-6, 0.0)
interval, beta = copula.computeUnilateralConfidenceIntervalWithMarginalProbability(
    0.95, False
)
ott.assert_almost_equal(interval.getLowerBound(), [0.0, 0.0], 0.0, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [0.974144, 0.974144], 1e-6, 0.0)
ott.assert_almost_equal(beta, 0.974144, 1e-6, 0.0)
interval, beta = copula.computeUnilateralConfidenceIntervalWithMarginalProbability(
    0.95, True
)
ott.assert_almost_equal(interval.getLowerBound(), [0.0258555, 0.0258555], 1e-6, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [1.0, 1.0], 0.0, 0.0)
ott.assert_almost_equal(beta, 0.974144, 1e-6, 0.0)

# Extract the marginals
for i in range(dim):
    margin = copula.getMarginal(i)
    assert margin.getImplementation().getClassName() == "IndependentCopula"
    ott.assert_almost_equal(margin.computePDF(ot.Point(1, 0.25)), 1.0, 0.0, 0.0)
    ott.assert_almost_equal(margin.computeCDF(ot.Point(1, 0.25)), 0.25, 0.0, 0.0)
    ott.assert_almost_equal(margin.computeQuantile(0.95), [0.95], 0.0, 0.0)

margin0Realization = copula.getMarginal(0).getRealization()
ott.assert_almost_equal(margin0Realization, [0.28199], 1e-5, 0.0)
margin1Realization = copula.getMarginal(1).getRealization()
ott.assert_almost_equal(margin1Realization, [0.814497], 1e-5, 0.0)

# Extract a 2-D marginal
indices = [1, 0]
assert indices == [1, 0]
margins = copula.getMarginal(indices)
assert margins.getImplementation().getClassName() == "FrankCopula"
ott.assert_almost_equal(margins.computePDF(ot.Point(2, 0.25)), 1.334391, 1e-5, 0.0)
ott.assert_almost_equal(margins.computeCDF(ot.Point(2, 0.25)), 0.107307, 1e-5, 0.0)
quantile = ot.Point(margins.computeQuantile(0.95))
ott.assert_almost_equal(quantile, [0.974144, 0.974144], 1e-5, 0.0)
ott.assert_almost_equal(margins.computeCDF(quantile), 0.95, 1e-5, 0.0)
marginsRealization = margins.getRealization()
ott.assert_almost_equal(marginsRealization, [0.638269, 0.49089], 1e-5, 0.0)

# Test computeArchimedeanGeneratorDerivative near 0 (regression test for sign bug)
t_small = 1e-6
deriv = copula.computeArchimedeanGeneratorDerivative(t_small)
# Should be negative (bug was positive sign)
assert deriv < 0.0
# As t->0, phi'(t) -> -1/t for any finite theta
ott.assert_almost_equal(deriv, -1.0 / t_small, 1e-3, 0.0)

ot.Log.Show(ot.Log.TRACE)
validation = ott.DistributionValidation(copula)
validation.run()

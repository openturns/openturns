#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Instantiate one distribution object
dim = 2
copula = ot.MarshallOlkinCopula()
assert copula.getClassName() == "MarshallOlkinCopula"
ott.assert_almost_equal(copula.getMean(), [0.5, 0.5], 0.0, 0.0)
ott.assert_almost_equal(
    copula.getCovariance(),
    ot.CovarianceMatrix(2, [0.0833333, 0.0357144, 0.0357144, 0.0833333]),
    1e-6,
    0.0,
)

# Is this copula an elliptical distribution?
assert not copula.isElliptical()

# Is this copula elliptical ?
assert not copula.hasEllipticalCopula()

# Is this copula independent ?
assert not copula.hasIndependentCopula()
assert not copula.isContinuous()

# Test for specific accessors
ott.assert_almost_equal(copula.getAlpha(), 0.5, 0.0, 0.0)
ott.assert_almost_equal(copula.getBeta(), 0.5, 0.0, 0.0)

# Test for realization of distribution
oneRealization = copula.getRealization()
ott.assert_almost_equal(oneRealization, [0.396744, 0.779345], 1e-4, 0.0)

# Test for sampling
size = 10
oneSample = copula.getSample(size)
ott.assert_almost_equal(
    oneSample,
    [
        [0.939781, 0.939781],
        [0.847651, 0.253049],
        [0.146966, 0.510342],
        [0.780578, 0.780578],
        [0.67373, 0.862132],
        [0.468643, 0.685629],
        [0.91154, 0.346468],
        [0.0442859, 0.436561],
        [0.175493, 0.175493],
        [0.964012, 0.913177],
    ],
    1e-4,
    0.0,
)

# Test for sampling
size = 10000
anotherSample = copula.getSample(size)
ott.assert_almost_equal(anotherSample.computeMean(), [0.502468, 0.500984], 1e-4, 0.0)
ott.assert_almost_equal(
    anotherSample.computeCovariance(),
    ot.CovarianceMatrix(2, [0.0842064, 0.0362275, 0.0362275, 0.0843318]),
    1e-4,
    0.0,
)

# Define a point
point = ot.Point(dim, 0.2)

# Show CDF of point
pointCDF = copula.computeCDF(point)
ott.assert_almost_equal(point, [0.2, 0.2], 0.0, 0.0)
ott.assert_almost_equal(pointCDF, 0.089442719099991602, 1e-12, 0.0)

# Get 50% quantile
quantile = copula.computeQuantile(0.5)
ott.assert_almost_equal(quantile, [0.629961, 0.629961], 1e-6, 0.0)
ott.assert_almost_equal(copula.computeCDF(quantile), 0.5, 1e-6, 0.0)

# Get 5% quantile
quantile = copula.computeQuantile(0.95, True)
ott.assert_almost_equal(quantile, [0.135721, 0.135721], 1e-6, 0.0)

# Get 95% survival function
inverseSurvival = ot.Point(copula.computeInverseSurvivalFunction(0.95))
ott.assert_almost_equal(inverseSurvival, [0.027249037839104637, 0.027249037839104637], 1e-6, 0.0)
ott.assert_almost_equal(
    copula.computeSurvivalFunction(inverseSurvival), 0.95, 1e-6, 0.0
)
# Confidence regions
"""
Fails, because of an INF in the PDF.
interval, threshold = copula.computeMinimumVolumeIntervalWithMarginalProbability(0.95)
"""
interval, beta = copula.computeUnilateralConfidenceIntervalWithMarginalProbability(
    0.95, False
)
ott.assert_almost_equal(interval.getLowerBound(), [0.0, 0.0], 0.0, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [0.966383, 0.966383], 1e-6, 0.0)
ott.assert_almost_equal(beta, 0.966383, 1e-6, 0.0)
interval, beta = copula.computeUnilateralConfidenceIntervalWithMarginalProbability(
    0.95, True
)
ott.assert_almost_equal(interval.getLowerBound(), [0.027249037839104637, 0.027249037839104637], 1e-6, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [1.0, 1.0], 0.0, 0.0)
ott.assert_almost_equal(beta, 0.972751, 1e-6, 0.0)

# Extract the marginals
for i in range(dim):
    margin = copula.getMarginal(i)
    assert margin.getImplementation().getClassName() == "IndependentCopula"
    ott.assert_almost_equal(margin.computePDF(ot.Point(1, 0.25)), 1.0, 0.0, 0.0)
    ott.assert_almost_equal(margin.computeCDF(ot.Point(1, 0.25)), 0.25, 0.0, 0.0)
    ott.assert_almost_equal(margin.computeQuantile(0.95), [0.95], 0.0, 0.0)

margin0Realization = copula.getMarginal(0).getRealization()
ott.assert_almost_equal(margin0Realization, [0.0998232], 1e-6, 0.0)
margin1Realization = copula.getMarginal(1).getRealization()
ott.assert_almost_equal(margin1Realization, [0.33856], 1e-6, 0.0)

# Extract a 2-D marginal
indices = [1, 0]
assert indices == [1, 0]
margins = copula.getMarginal(indices)
assert margins.getImplementation().getClassName() == "MarginalDistribution"
ott.assert_almost_equal(margins.computeCDF(ot.Point(2, 0.25)), 0.125, 1e-12, 0.0)
quantile = ot.Point(margins.computeQuantile(0.95))
ott.assert_almost_equal(quantile, [0.966383, 0.966383], 1e-6, 0.0)
ott.assert_almost_equal(margins.computeCDF(quantile), 0.95, 1e-6, 0.0)
marginsRealization = margins.getRealization()
ott.assert_almost_equal(marginsRealization, [0.29664, 0.868128], 1e-5, 0.0)

# Dependence measures in the three regimes
print("kendall=", copula.getKendallTau())
print("spearman=", copula.getSpearmanCorrelation())
ott.assert_almost_equal(copula.getKendallTau()[1, 0], 1.0 / 3.0)
ott.assert_almost_equal(copula.getSpearmanCorrelation()[1, 0], 3.0 / 7.0)
independent_copula = ot.MarshallOlkinCopula(0.0, 0.5)
print("independent kendall=", independent_copula.getKendallTau())
print("independent spearman=", independent_copula.getSpearmanCorrelation())
ott.assert_almost_equal(independent_copula.getKendallTau()[1, 0], 0.0)
ott.assert_almost_equal(independent_copula.getSpearmanCorrelation()[1, 0], 0.0)
print("independent=", independent_copula.hasIndependentCopula())
min_copula = ot.MarshallOlkinCopula(1.0, 1.0)
print("min kendall=", min_copula.getKendallTau())
print("min spearman=", min_copula.getSpearmanCorrelation())
ott.assert_almost_equal(min_copula.getKendallTau()[1, 0], 1.0)
ott.assert_almost_equal(min_copula.getSpearmanCorrelation()[1, 0], 1.0)

# Realizations in the three regimes stay within the unit square
for reg in [copula, independent_copula, min_copula]:
    realization = reg.getRealization()
    assert 0.0 <= realization[0] <= 1.0 and 0.0 <= realization[1] <= 1.0
    print("realization=", repr(realization))

# Continuity
print("continuous=", copula.isContinuous())
print("continuous(0, 0)=", ot.MarshallOlkinCopula(0.0, 0.0).isContinuous())

# CDF on the boundary
ott.assert_almost_equal(copula.computeCDF([0.0, 0.5]), 0.0)
ott.assert_almost_equal(copula.computeCDF([0.0, 0.0]), 0.0)

# Error cases
with ott.assert_raises(TypeError):
    copula.setParameter([0.5])
with ott.assert_raises(TypeError):
    ot.MarshallOlkinCopula(2.0, 0.5)
with ott.assert_raises(TypeError):
    copula.setAlpha(2.0)
with ott.assert_raises(TypeError):
    copula.setBeta(-0.1)

# --- MarshallOlkinCopula: continuity matches independence ---
assert ot.MarshallOlkinCopula(0.0, 0.5).isContinuous()
assert not ot.MarshallOlkinCopula(0.5, 0.5).isContinuous()
with ott.assert_raises(Exception):
    ot.MarshallOlkinCopula(0.5, 0.5).computeCDF([0.5])

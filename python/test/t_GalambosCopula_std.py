#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

ot.RandomGenerator.SetSeed(0)

# Instantiate one distribution object
dim = 2
copula = ot.GalambosCopula(0.5)
ott.assert_almost_equal(copula.getMean(), [0.5, 0.5], 0.0, 0.0)
ref_cov_matrix = ot.CovarianceMatrix(2, [0.0833333, 0.0241064, 0.0241064, 0.0833333])
ott.assert_almost_equal(copula.getCovariance(), ref_cov_matrix, 1e-5, 0.0)

# Is this copula an elliptical distribution?
assert not copula.isElliptical()

# Is this copula elliptical?
assert not copula.hasEllipticalCopula()

# Is this copula independent?
assert not copula.hasIndependentCopula()

# Test for realization of distribution
ott.assert_almost_equal(copula.getRealization(), [0.629877, 0.871724], 1e-5, 0.0)

# Test correlation and covariance
ott.assert_almost_equal(
    copula.getCorrelation(),
    ot.CovarianceMatrix(2, [1, 0.289277, 0.289277, 1]),
    1e-5,
    0.0,
)
ott.assert_almost_equal(
    copula.getCovariance(),
    ot.CovarianceMatrix(2, [0.0833333, 0.0241064, 0.0241064, 0.0833333]),
    1e-5,
    0.0,
)
ott.assert_almost_equal(
    copula.getSpearmanCorrelation(),
    ot.CovarianceMatrix(2, [1, 0.289277, 0.289277, 1]),
    1e-5,
    0.0,
)
ott.assert_almost_equal(
    copula.getKendallTau(),
    ot.CovarianceMatrix(2, [1, 0.299108, 0.299108, 1]),
    1e-5,
    0.0,
)

# Test for sampling
oneSample = copula.getSample(10)
ref_sample = [
    [0.135276, 0.0205545],
    [0.347057, 0.937166],
    [0.92068, 0.699802],
    [0.0632061, 0.188369],
    [0.714382, 0.437797],
    [0.373767, 0.67536],
    [0.883503, 0.441284],
    [0.92851, 0.925177],
    [0.684575, 0.834214],
    [0.359802, 0.916859],
]
ott.assert_almost_equal(oneSample, ref_sample, 1e-5, 0.0)

# Test for sampling
anotherSample = copula.getSample(10000)
ott.assert_almost_equal(anotherSample.computeMean(), [0.502777, 0.501261], 1e-5, 0.0)
ref_cov_matrix_2 = ot.CovarianceMatrix(2, [0.0833333, 0.0241064, 0.0244522, 0.0833333])
ott.assert_almost_equal(copula.getCovariance(), ref_cov_matrix_2, 1e-5, 0.0)

# Define a point
point = ot.Point(dim, 0.2)

# Show PDF and CDF of point
ott.assert_almost_equal(copula.computePDF(point), 1.231981, 1e-5, 0.0)
ott.assert_almost_equal(copula.computeCDF(point), 0.059814, 1e-4, 0.0)

# Get 50% quantile
quantile = copula.computeQuantile(0.5)
ott.assert_almost_equal(quantile, [0.67295, 0.67295], 1e-4, 0.0)
ott.assert_almost_equal(copula.computeCDF(quantile), 0.500000, 1e-4, 0.0)

# Get 5% quantile
quantile = copula.computeQuantile(0.95, True)
ott.assert_almost_equal(quantile, [0.180532, 0.180532], 1e-5, 0.0)

# Get 95% survival function
inverseSurvival = ot.Point(copula.computeInverseSurvivalFunction(0.95))
ott.assert_almost_equal(inverseSurvival, [0.0258323, 0.0258323], 1e-4, 0.0)
ott.assert_almost_equal(
    copula.computeSurvivalFunction(inverseSurvival), 0.950000, 1e-4, 0.0
)
ott.assert_almost_equal(copula.computeEntropy(), -0.062336, 1e-4, 0.0)

# Test conditional methods
y = [0.5]
x = 0.6
p = copula.computeConditionalCDF(x, y)
ott.assert_almost_equal(p, 0.62707, 1e-4, 0.0)
ott.assert_almost_equal(
    copula.computeConditionalQuantile(p, y), 0.5999999999999999, 1e-4, 0.0
)

# Extract the marginals
margin_0 = copula.getMarginal(0)
ott.assert_almost_equal(margin_0.computePDF([0.25]), 1.0, 0.0, 0.0)
ott.assert_almost_equal(margin_0.computeCDF([0.25]), 0.25, 0.0, 0.0)
ott.assert_almost_equal(margin_0.computeQuantile(0.95), [0.95], 0.0, 0.0)
ott.assert_almost_equal(margin_0.getRealization(), [0.56939], 1e-5, 0.0)
margin_1 = copula.getMarginal(1)
ott.assert_almost_equal(margin_1.computePDF([0.25]), 1.0, 0.0, 0.0)
ott.assert_almost_equal(margin_1.computeCDF([0.25]), 0.25, 0.0, 0.0)
ott.assert_almost_equal(margin_1.computeQuantile(0.95), [0.95], 0.0, 0.0)
ott.assert_almost_equal(margin_1.getRealization(), [0.0571768], 1e-5, 0.0)

# Extract a 2-D marginal
margins = copula.getMarginal([1, 0])
ott.assert_almost_equal(margins.computePDF([0.25] * 2), 1.1784, 1e-4, 0.0)
ott.assert_almost_equal(margins.computeCDF([0.25] * 2), 0.0883883, 1e-5, 0.0)
quantile = ot.Point(margins.computeQuantile(0.95))
ott.assert_almost_equal(quantile, [0.971115, 0.971115], 1e-5, 0.0)
ott.assert_almost_equal(margins.computeCDF(quantile), 0.950000, 1e-4, 0.0)
ott.assert_almost_equal(margins.getRealization(), [0.915787, 0.243714], 1e-5, 0.0)

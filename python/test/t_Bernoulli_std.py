#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()


# Instantiate one distribution object
distribution = ot.Bernoulli(0.7)
assert distribution.getClassName() == "Bernoulli"

# Is this distribution elliptical ?
assert not distribution.isElliptical()

# Is this distribution continuous ?
assert not distribution.isContinuous()

# Test for realization of distribution
oneRealization = distribution.getRealization()
ott.assert_almost_equal(oneRealization, [1.0], 0.0, 0.0)

# Define a point
point = ot.Point(distribution.getDimension(), 0.0)
ott.assert_almost_equal(point, [0.0], 0.0, 0.0)

# PDF value
PDF = distribution.computePDF(point)
ott.assert_almost_equal(PDF, 0.3, 1e-6, 0.0)

# CDF value
CDF = distribution.computeCDF(point)
ott.assert_almost_equal(CDF, 0.3, 1e-6, 0.0)

# quantile
quantile = distribution.computeQuantile(0.95)
ott.assert_almost_equal(quantile, [1.0], 0.0, 0.0)
ott.assert_almost_equal(distribution.computeCDF(quantile), 1.0, 1e-6, 0.0)
ott.assert_almost_equal(distribution.computeEntropy(), 0.610864, 1e-6, 0.0)
mean = distribution.getMean()
ott.assert_almost_equal(mean, [0.7], 0.0, 0.0)
standardDeviation = distribution.getStandardDeviation()
ott.assert_almost_equal(standardDeviation, [0.458258], 1e-6, 0.0)
skewness = distribution.getSkewness()
ott.assert_almost_equal(skewness, [-0.872872], 1e-6, 0.0)
kurtosis = distribution.getKurtosis()
ott.assert_almost_equal(kurtosis, [1.761905], 1e-6, 0.0)
covariance = distribution.getCovariance()
ott.assert_almost_equal(covariance, ot.CovarianceMatrix(1, [0.21]), 1e-12, 0.0)
parameters = distribution.getParametersCollection()
assert len(parameters) == 1
ott.assert_almost_equal(parameters[0], [0.7], 0.0, 0.0)
assert distribution.getStandardRepresentative().getImplementation().getClassName() == "Bernoulli"
ott.assert_almost_equal(distribution.getProbabilities(), [0.3, 0.7], 1e-12, 0.0)

ot.Log.Show(ot.Log.TRACE)
validation = ott.DistributionValidation(distribution)
validation.run()

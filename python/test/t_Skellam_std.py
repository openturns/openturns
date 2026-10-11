#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Instantiate one distribution object
distribution = ot.Skellam(10.0, 5.0)
assert distribution.getClassName() == "Skellam"

# Is this distribution elliptical ?
assert not distribution.isElliptical()

# Is this distribution continuous ?
assert not distribution.isContinuous()

# Test for realization of distribution
oneRealization = distribution.getRealization()
ott.assert_almost_equal(oneRealization, [11.0], 0.0, 0.0)

# Define a point
point = ot.Point(distribution.getDimension(), 12.0)
ott.assert_almost_equal(point, [12.0], 0.0, 0.0)

# Show PDF and CDF of point
LPDF = distribution.computeLogPDF(point)
ott.assert_almost_equal(LPDF, -3.91245138189, 1e-9, 0.0)
PDF = distribution.computePDF(point)
ott.assert_almost_equal(PDF, 0.0199914343056, 1e-9, 0.0)
ott.assert_almost_equal(PDF, distribution.computeCDF(point) - distribution.computeCDF([point[0] - 1.0]), 1e-12, 0.0)

CDF = distribution.computeCDF(point)
ott.assert_almost_equal(CDF, 0.971360859022, 1e-9, 0.0)
CCDF = distribution.computeComplementaryCDF(point)
ott.assert_almost_equal(CCDF, 0.028639140978, 1e-9, 0.0)
CF = distribution.computeCharacteristicFunction(point[0])
ott.assert_almost_equal([CF.real], [-0.0861799503022], 1e-9, 0.0)
ott.assert_almost_equal([CF.imag], [-0.0425613135496], 1e-9, 0.0)
LCF = distribution.computeLogCharacteristicFunction(point[0])
ott.assert_almost_equal([LCF.real], [-2.34219061901], 1e-9, 0.0)
ott.assert_almost_equal([LCF.imag], [-2.68286459], 1e-9, 0.0)
GF = distribution.computeGeneratingFunction(0.3 + 0.7j)
ott.assert_almost_equal([GF.real], [4.64245238616e-05], 1e-9, 0.0)
ott.assert_almost_equal([GF.imag], [6.70957137037e-05], 1e-9, 0.0)
LGF = distribution.computeLogGeneratingFunction(0.3 + 0.7j)
ott.assert_almost_equal([LGF.real], [-9.41379310345], 1e-9, 0.0)
ott.assert_almost_equal([LGF.imag], [0.965517241379], 1e-9, 0.0)
quantile = distribution.computeQuantile(0.95)
ott.assert_almost_equal(quantile, [11.0], 0.0, 0.0)
ott.assert_almost_equal(distribution.computeCDF(quantile), 0.951369424716, 1e-9, 0.0)
ott.assert_almost_equal(distribution.computeEntropy(), 2.772305, 1e-6, 0.0)
mean = distribution.getMean()
ott.assert_almost_equal(mean, [5.0], 0.0, 0.0)
standardDeviation = distribution.getStandardDeviation()
ott.assert_almost_equal(standardDeviation, [3.87298], 1e-5, 0.0)
skewness = distribution.getSkewness()
ott.assert_almost_equal(skewness, [0.0860663], 1e-5, 0.0)
kurtosis = distribution.getKurtosis()
ott.assert_almost_equal(kurtosis, [3.06667], 1e-5, 0.0)
covariance = distribution.getCovariance()
ott.assert_almost_equal(covariance, ot.CovarianceMatrix(1, [15.0]), 1e-12, 0.0)
parameters = distribution.getParametersCollection()
assert len(parameters) == 1
ott.assert_almost_equal(parameters[0], [10.0, 5.0], 0.0, 0.0)
standard = distribution.getStandardRepresentative()
assert standard.getImplementation().getClassName() == "Skellam"

ot.Log.Show(ot.Log.TRACE)
validation = ott.DistributionValidation(distribution)
validation.run()

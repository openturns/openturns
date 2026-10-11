#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()


# Instantiate one distribution object
distribution = ot.Hypergeometric(25, 7, 14)
assert distribution.getClassName() == "Hypergeometric"

# Is this distribution elliptical ?
assert not distribution.isElliptical()

# Is this distribution continuous ?
assert not distribution.isContinuous()

# Test for realization of distribution
oneRealization = distribution.getRealization()
ott.assert_almost_equal(oneRealization, [2.0], 0.0, 0.0)

# Define a point
point = ot.Point(distribution.getDimension(), 5.0)
ott.assert_almost_equal(point, [5.0], 0.0, 0.0)

# Show PDF and CDF of point
LPDF = distribution.computeLogPDF(point)
ott.assert_almost_equal(LPDF, -1.47376350877, 1e-9, 0.0)
PDF = distribution.computePDF(point)
ott.assert_almost_equal(PDF, 0.229061784897, 1e-9, 0.0)

CDF = distribution.computeCDF(point)
ott.assert_almost_equal(CDF, 0.92414187643, 1e-9, 0.0)
CCDF = distribution.computeComplementaryCDF(point)
ott.assert_almost_equal(CCDF, 0.0758581235698, 1e-9, 0.0)
CF = distribution.computeCharacteristicFunction(point[0])
ott.assert_almost_equal([CF.real], [0.112691702824], 1e-9, 0.0)
ott.assert_almost_equal([CF.imag], [0.314426247514], 1e-9, 0.0)
LCF = distribution.computeLogCharacteristicFunction(point[0])
ott.assert_almost_equal([LCF.real], [-1.09658163526], 1e-9, 0.0)
ott.assert_almost_equal([LCF.imag], [1.22665413395], 1e-9, 0.0)
GF = distribution.computeGeneratingFunction(0.3 + 0.7j)
ott.assert_almost_equal([GF.real], [-0.0767726758078], 1e-9, 0.0)
ott.assert_almost_equal([GF.imag], [-0.123723016677], 1e-9, 0.0)
LGF = distribution.computeLogGeneratingFunction(0.3 + 0.7j)
ott.assert_almost_equal([LGF.real], [-1.92684335624], 1e-9, 0.0)
ott.assert_almost_equal([LGF.imag], [-2.12616798622], 1e-9, 0.0)
quantile = distribution.computeQuantile(0.95)
ott.assert_almost_equal(quantile, [6.0], 0.0, 0.0)
ott.assert_almost_equal(distribution.computeCDF(quantile), 0.992860411899, 1e-9, 0.0)
ott.assert_almost_equal(distribution.computeEntropy(), 1.546540, 1e-6, 0.0)
mean = distribution.getMean()
ott.assert_almost_equal(mean, [3.92], 1e-12, 0.0)
standardDeviation = distribution.getStandardDeviation()
ott.assert_almost_equal(standardDeviation, [1.13737], 1e-5, 0.0)
skewness = distribution.getSkewness()
ott.assert_almost_equal(skewness, [-0.0504599], 1e-5, 0.0)
kurtosis = distribution.getKurtosis()
ott.assert_almost_equal(kurtosis, [2.83749], 1e-5, 0.0)
covariance = distribution.getCovariance()
ott.assert_almost_equal(covariance, ot.CovarianceMatrix(1, [1.2936]), 1e-4, 0.0)
parameters = distribution.getParametersCollection()
assert len(parameters) == 1
ott.assert_almost_equal(parameters[0], [25.0, 7.0, 14.0], 0.0, 0.0)
standard = distribution.getStandardRepresentative()
assert standard.getImplementation().getClassName() == "Hypergeometric"

# should not hang when the range is [0] (dirac)
ot.Hypergeometric(25, 0, 0).computeScalarQuantile(0.6)

ot.Log.Show(ot.Log.TRACE)
ot.RandomGenerator.SetSeed(1)
validation = ott.DistributionValidation(distribution)
validation.run()

# test sampling with rHypergeometric
n, k, m = 5, 2, 1
N = int(1e5)
sample = ot.Sample([[ot.DistFunc.rHypergeometric(n, k, m)] for _ in range(N)])
distribution = ot.Hypergeometric(n, k, m)
result = ot.FittingTest.ChiSquared(sample, distribution, 0.01)
assert result.getBinaryQualityMeasure()

#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Instantiate one distribution object
distribution = ot.ZipfMandelbrot(15, 1.2, 2.0)
print("Distribution ", repr(distribution))
print("Distribution ", distribution)

# Is this distribution elliptical ?
print("Elliptical = ", distribution.isElliptical())

# Is this distribution continuous ?
print("Continuous = ", distribution.isContinuous())

# Test for realization of distribution
oneRealization = distribution.getRealization()
print("oneRealization=", repr(oneRealization))

# Test for getRange
print("getRange=", repr(distribution.getRange()))

# Define a point
point = ot.Point(distribution.getDimension(), 5.0)
print("Point= ", repr(point))

# PDF value
PDF = distribution.computePDF(point)
print("pdf     =%.6f" % PDF)


# derivative of the PDF with regards the parameters of the distribution
CDF = distribution.computeCDF(point)
print("cdf=%.6f" % CDF)
# quantile
quantile = distribution.computeQuantile(0.95)
print("quantile=", repr(quantile))
print("cdf(quantile)=%.6f" % distribution.computeCDF(quantile))
print("entropy=%.6f" % distribution.computeEntropy())
mean = distribution.getMean()
print("mean=", repr(mean))
standardDeviation = distribution.getStandardDeviation()
print("standard deviation=", repr(standardDeviation))
skewness = distribution.getSkewness()
print("skewness=", repr(skewness))
kurtosis = distribution.getKurtosis()
print("kurtosis=", repr(kurtosis))
covariance = distribution.getCovariance()
print("covariance=", repr(covariance))
parameters = distribution.getParametersCollection()
print("parameters=", repr(parameters))
print("Standard representative=", distribution.getStandardRepresentative())

# Bug fix: CDF at non-integer k between n_-1 and n_ must be < 1
# CDF(14.5) should equal CDF(14) = H(14)/H(15), not 1.0
cdf_14 = distribution.computeCDF([14.0])
cdf_145 = distribution.computeCDF([14.5])
ott.assert_almost_equal(cdf_145, cdf_14, 1e-12, 1e-12)
assert cdf_145 < 1.0, "CDF at 14.5 must be < 1.0"

# Bug fix: CDF at non-integer k must use floor, not round
# CDF(13.5) should equal CDF(13), not CDF(14)
cdf_13 = distribution.computeCDF([13.0])
cdf_135 = distribution.computeCDF([13.5])
cdf_14 = distribution.computeCDF([14.0])
ott.assert_almost_equal(cdf_135, cdf_13, 1e-12, 1e-12)
assert abs(cdf_135 - cdf_14) > 1e-6, "CDF at 13.5 must differ from CDF at 14"

# Bug fix: setParameter with negative n must throw
with ott.assert_raises(TypeError):
    dist_bad = ot.ZipfMandelbrot()
    dist_bad.setParameter([-1.0, 0.0, 1.0])

# Bug fix: isElliptical must be false for discrete distribution
assert not distribution.isElliptical()

ot.Log.Show(ot.Log.TRACE)
validation = ott.DistributionValidation(distribution)
validation.run()

#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Instantiate one distribution object
distribution = ot.Geometric(0.7)
assert distribution.getClassName() == "Geometric"

# Is this distribution elliptical ?
assert not distribution.isElliptical()

# Is this distribution continuous ?
assert not distribution.isContinuous()

# Test for realization of distribution
oneRealization = distribution.getRealization()
ott.assert_almost_equal(oneRealization, [1.0], 0.0, 0.0)

# Define a point
point = ot.Point(distribution.getDimension(), 3.0)
ott.assert_almost_equal(point, [3.0], 0.0, 0.0)

# Show PDF and CDF of point
eps = 1e-5
# PDF value
PDF = distribution.computePDF(point)
ott.assert_almost_equal(PDF, 0.063, 1e-6, 0.0)

# derivative of the PDF with regards the parameters of the distribution
CDF = distribution.computeCDF(point)
ott.assert_almost_equal(CDF, 0.973, 1e-6, 0.0)
PDFgr = distribution.computePDFGradient(point)
ott.assert_almost_equal(PDFgr, [-0.33], 1e-12, 0.0)

# derivative of the PDF with regards the parameters of the distribution
CDFgr = distribution.computeCDFGradient(point)
ott.assert_almost_equal(CDFgr, [0.27], 1e-12, 0.0)

# quantile
quantile = distribution.computeQuantile(0.95)
ott.assert_almost_equal(quantile, [3.0], 0.0, 0.0)
ott.assert_almost_equal(distribution.computeCDF(quantile), 0.973, 1e-6, 0.0)
ott.assert_almost_equal(distribution.computeEntropy(), 0.872663, 1e-6, 0.0)
mean = distribution.getMean()
ott.assert_almost_equal(mean, [1.42857], 1e-5, 0.0)
standardDeviation = distribution.getStandardDeviation()
ott.assert_almost_equal(standardDeviation, [0.782461], 1e-6, 0.0)
skewness = distribution.getSkewness()
ott.assert_almost_equal(skewness, [2.37346], 1e-5, 0.0)
kurtosis = distribution.getKurtosis()
ott.assert_almost_equal(kurtosis, [10.6333], 1e-4, 0.0)
covariance = distribution.getCovariance()
ott.assert_almost_equal(covariance, ot.CovarianceMatrix(1, [0.612245]), 1e-6, 0.0)
parameters = distribution.getParametersCollection()
assert len(parameters) == 1
ott.assert_almost_equal(parameters[0], [0.7], 0.0, 0.0)
standard = distribution.getStandardRepresentative()
assert standard.getImplementation().getClassName() == "Geometric"

# computeProbability regression test for discrete distributions
p = ot.Geometric(0.5).computeProbability(ot.Interval(2, 6))
ott.assert_almost_equal(p, 0.484375)

ot.Log.Show(ot.Log.TRACE)
validation = ott.DistributionValidation(distribution)
validation.run()

# Test getSupport does not include k=0 (support starts at 1)
dist_geo = ot.Geometric(0.5)
support = dist_geo.getSupport(ot.Interval(-1.0, 0.5))
ott.assert_almost_equal(support.getSize(), 0)

# Test getSupport returns correct values for a valid interval
support = dist_geo.getSupport(ot.Interval(1.0, 3.0))
ott.assert_almost_equal(support, ot.Sample([[1.0], [2.0], [3.0]]))

# Test computePDFGradient at p=1, k=1 does not return NaN
dist_p1 = ot.Geometric(1.0)
pdf_grad = dist_p1.computePDFGradient([1.0])
assert not any(map(lambda x: x != x, pdf_grad)), "PDF gradient should not be NaN at p=1"

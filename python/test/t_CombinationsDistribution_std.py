#! /usr/bin/env python

import openturns.experimental as otexp
import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Instantiate one distribution object
distribution = otexp.CombinationsDistribution(5, 12)
print("Distribution ", distribution)

# Is this distribution elliptical ?
print("Elliptical = ", distribution.isElliptical())

# Is this distribution continuous ?
print("Continuous = ", distribution.isContinuous())

# Test for realization of distribution
oneRealization = distribution.getRealization()
print("oneRealization=", oneRealization)

# Test for sampling
size = 10000
oneSample = distribution.getSample(size)
print("oneSample first=", oneSample[0], " last=", oneSample[size - 1])
print("mean=", oneSample.computeMean())
print("covariance=", oneSample.computeCovariance())
# Define a point
point = ot.Point(distribution.getDimension(), 4.0)
print("Point= ", point)

# Show PDF and CDF of point
LPDF = distribution.computeLogPDF(point)
assert (LPDF == ot.SpecFunc.LowestScalar)
PDF = distribution.computePDF(point)
print("pdf     =", PDF)
CDF = distribution.computeCDF(point)
print("cdf=%.6f" % CDF)
CCDF = distribution.computeComplementaryCDF(point)
print("ccdf=%.6f" % CCDF)
quantile = distribution.computeQuantile(0.95)
print("quantile=", quantile)
print("cdf(quantile)=", distribution.computeCDF(quantile))
print("entropy=%.6f" % distribution.computeEntropy())
mean = distribution.getMean()
print("mean=", mean)
covariance = distribution.getCovariance()
print("covariance=", covariance)
parameters = distribution.getParametersCollection()
print("parameters=", parameters)

ot.Log.Show(ot.Log.TRACE)
validation = ott.DistributionValidation(distribution)
validation.skipCorrelation()  # slow
validation.run()

# check K,N accessors
distribution.setN(11)
distribution.setK(4)
ott.assert_almost_equal(distribution.getParameter(), [4, 11])

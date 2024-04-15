#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()


# Instantiate one distribution object
distribution = ot.Hypergeometric(25, 7, 14)
print("Distribution ", repr(distribution))
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

size = 100
for i in range(2):
    msg = ""
    if ot.FittingTest.ChiSquared(
        distribution.getSample(size), distribution
    ).getBinaryQualityMeasure():
        msg = "accepted"
    else:
        msg = "rejected"
    print("Chi2 test for the generator, sample size=", size, " is", msg)
    size *= 10

# Define a point
point = ot.Point(distribution.getDimension(), 5.0)
print("Point= ", point)

# Show PDF and CDF of point
eps = 1e-5
LPDF = distribution.computeLogPDF(point)
print("log pdf= %.12g" % LPDF)
PDF = distribution.computePDF(point)
print("pdf     = %.12g" % PDF)
print(
    "pdf (FD)= %.12g"
    % (
        distribution.computeCDF(point + ot.Point(1, 0))
        - distribution.computeCDF(point + ot.Point(1, -1))
    )
)
CDF = distribution.computeCDF(point)
print("cdf= %.12g" % CDF)
CCDF = distribution.computeComplementaryCDF(point)
print("ccdf= %.12g" % CCDF)
CF = distribution.computeCharacteristicFunction(point[0])
print("characteristic function= (%.12g%+.12gj)" % (CF.real, CF.imag))
LCF = distribution.computeLogCharacteristicFunction(point[0])
print("log characteristic function= (%.12g%+.12gj)" % (LCF.real, LCF.imag))
GF = distribution.computeGeneratingFunction(0.3 + 0.7j)
print("generating function= (%.12g%+.12gj)" % (GF.real, GF.imag))
LGF = distribution.computeLogGeneratingFunction(0.3 + 0.7j)
print("log generating function= (%.12g%+.12gj)" % (LGF.real, LGF.imag))
quantile = distribution.computeQuantile(0.95)
print("quantile=", quantile)
print("cdf(quantile)= %.12g" % distribution.computeCDF(quantile))
print("entropy=%.6f" % distribution.computeEntropy())
mean = distribution.getMean()
print("mean=", mean)
standardDeviation = distribution.getStandardDeviation()
print("standard deviation=", standardDeviation)
skewness = distribution.getSkewness()
print("skewness=", skewness)
kurtosis = distribution.getKurtosis()
print("kurtosis=", kurtosis)
covariance = distribution.getCovariance()
print("covariance=", covariance)
parameters = distribution.getParametersCollection()
print("parameters=", parameters)
print("Standard representative=", distribution.getStandardRepresentative())

# should not hang when the range is [0] (dirac)
ot.Hypergeometric(25, 0, 0).computeScalarQuantile(0.6)

# computeProba test with bound far away
p = distribution.computeProbability(ot.Interval(-ot.SpecFunc.MaxScalar, ot.SpecFunc.MaxScalar))
ott.assert_almost_equal(p, 1.0)

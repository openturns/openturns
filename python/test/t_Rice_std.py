#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# Instantiate one distribution object
distribution = ot.Rice(5, 4)
print("Distribution ", repr(distribution))
print("Distribution ", distribution)

# Is this distribution elliptical ?
print("Elliptical = ", distribution.isElliptical())

# Is this distribution continuous ?
print("Continuous = ", distribution.isContinuous())

# Test for realization of distribution
oneRealization = distribution.getRealization()
print("oneRealization=", repr(oneRealization))

# Test for sampling
size = 10000
oneSample = distribution.getSample(size)
print("oneSample first=", repr(oneSample[0]), " last=", repr(oneSample[size - 1]))
print("mean=", repr(oneSample.computeMean()))
print("covariance=", repr(oneSample.computeCovariance()))

size = 100
for i in range(2):
    msg = ""
    if ot.FittingTest.Kolmogorov(
        distribution.getSample(size), distribution
    ).getBinaryQualityMeasure():
        msg = "accepted"
    else:
        msg = "rejected"
    print("Kolmogorov test for the generator, sample size=", size, " is", msg)
    size *= 10

# Define a point
point = ot.Point(distribution.getDimension(), 1.0)
print("Point= ", repr(point))

# Show PDF and CDF of point
eps = 1e-5
PDF = distribution.computePDF(point)
print("pdf     = %.12g" % PDF)
print(
    "pdf (FD)= %.9f"
    % (
        (
            distribution.computeCDF(point + ot.Point(1, eps))
            - distribution.computeCDF(point + ot.Point(1, -eps))
        )
        / (2.0 * eps),
    )
)
CDF = distribution.computeCDF(point)
print("cdf= %.12g" % CDF)
# CF = distribution.computeCharacteristicFunction( point[0] )
# print "characteristic function=", CF
PDFgr = distribution.computePDFGradient(point)
print("pdf gradient     =", repr(PDFgr))

PDFgrFD = ot.Point(2)
PDFgrFD[0] = (
    ot.Rice(distribution.getBeta() + eps, distribution.getNu()).computePDF(point)
    - ot.Rice(distribution.getBeta() - eps, distribution.getNu()).computePDF(point)
) / (2.0 * eps)

PDFgrFD[1] = (
    ot.Rice(distribution.getBeta(), distribution.getNu() + eps).computePDF(point)
    - ot.Rice(distribution.getBeta(), distribution.getNu() - eps).computePDF(point)
) / (2.0 * eps)
print("pdf gradient (FD)=", repr(PDFgrFD))
CDFgr = distribution.computeCDFGradient(point)
print("cdf gradient     =", repr(CDFgr))
CDFgrFD = ot.Point(2)
CDFgrFD[0] = (
    ot.Rice(distribution.getBeta() + eps, distribution.getNu()).computeCDF(point)
    - ot.Rice(distribution.getBeta() - eps, distribution.getNu()).computeCDF(point)
) / (2.0 * eps)

CDFgrFD[1] = (
    ot.Rice(distribution.getBeta(), distribution.getNu() + eps).computeCDF(point)
    - ot.Rice(distribution.getBeta(), distribution.getNu() - eps).computeCDF(point)
) / (2.0 * eps)
print("cdf gradient (FD)=", repr(CDFgrFD))
quantile = distribution.computeQuantile(0.95)
print("quantile=", repr(quantile))
print("cdf(quantile)= %.12g" % distribution.computeCDF(quantile))
# Get 95% survival function
inverseSurvival = ot.Point(distribution.computeInverseSurvivalFunction(0.95))
print("InverseSurvival=", repr(inverseSurvival))
print(
    "Survival(inverseSurvival)=%.6f"
    % distribution.computeSurvivalFunction(inverseSurvival)
)
print("entropy=%.6f" % distribution.computeEntropy())

# Confidence regions
interval, threshold = distribution.computeMinimumVolumeIntervalWithMarginalProbability(
    0.95
)
print("Minimum volume interval=", interval)
print("threshold=", ot.Point(1, threshold))
levelSet, beta = distribution.computeMinimumVolumeLevelSetWithThreshold(0.95)
print("Minimum volume level set=", levelSet)
print("beta=", ot.Point(1, beta))
interval, beta = distribution.computeBilateralConfidenceIntervalWithMarginalProbability(
    0.95
)
print("Bilateral confidence interval=", interval)
print("beta=", ot.Point(1, beta))
(
    interval,
    beta,
) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, False)
print("Unilateral confidence interval (lower tail)=", interval)
print("beta=", ot.Point(1, beta))
(
    interval,
    beta,
) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, True)
print("Unilateral confidence interval (upper tail)=", interval)
print("beta=", ot.Point(1, beta))

mean = distribution.getMean()
print("mean=", repr(mean))
covariance = distribution.getCovariance()
print("covariance=", repr(covariance))
parameters = distribution.getParametersCollection()
print("parameters=", repr(parameters))
print("Standard representative=", distribution.getStandardRepresentative())

# Specific to this distribution
nu = distribution.getNu()
print("nu=", nu)
standardDeviation = distribution.getStandardDeviation()
print("standard deviation=", repr(standardDeviation))
skewness = distribution.getSkewness()
print("skewness=", repr(skewness))
kurtosis = distribution.getKurtosis()
print("kurtosis=", repr(kurtosis))

#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Instantiate one distribution object
dimension = 3
meanPoint = ot.Point([0.5, -0.5, 1])
sigma = [2, 3, 1]

sample = ot.Sample(0, dimension)
# Create a collection of distribution
aCollection = ot.DistributionCollection()

aCollection.add(ot.Normal(meanPoint, sigma, ot.IdentityMatrix(dimension)))
sample.add(meanPoint)
meanPoint += [1.0] * dimension
aCollection.add(ot.Normal(meanPoint, sigma, ot.IdentityMatrix(dimension)))
sample.add(meanPoint)
meanPoint += [1.0] * dimension
aCollection.add(ot.Normal(meanPoint, sigma, ot.IdentityMatrix(dimension)))
sample.add(meanPoint)

# Instantiate one distribution object
distribution = ot.KernelMixture(ot.Normal(), sigma, sample)
print("Distribution ", repr(distribution))
print("Distribution ", distribution)
distributionRef = ot.Mixture(aCollection)

# Is this distribution elliptical ?
print("Elliptical = ", distribution.isElliptical())

# Is this distribution continuous ?
print("Continuous = ", distribution.isContinuous())

# Test for realization of distribution
oneRealization = distribution.getRealization()
print("oneRealization=", repr(oneRealization))

# Test for sampling
size = 100
oneSample = distribution.getSample(size)
print("oneSample first=", repr(oneSample[0]), " last=", repr(oneSample[size - 1]))
print("mean=", repr(oneSample.computeMean()))
print("covariance=", repr(oneSample.computeCovariance()))

# Define a point
point = [1.0] * distribution.getDimension()
print("Point= ", repr(point))

# Show PDF and CDF of point
eps = 1e-5

# derivative of PDF with regards its arguments
DDF = distribution.computeDDF(point)
print("ddf     =", repr(DDF))
print("ddf (ref)=", repr(distributionRef.computeDDF(point)))

# PDF value
LPDF = distribution.computeLogPDF(point)
print("log pdf=%.6f" % LPDF)
PDF = distribution.computePDF(point)
print("pdf     =%.6f" % PDF)
print("pdf (ref)=%.6f" % distributionRef.computePDF(point))

# derivative of the PDF with regards the parameters of the distribution
CDF = distribution.computeCDF(point)
print("cdf=%.6f" % CDF)
CCDF = distribution.computeComplementaryCDF(point)
print("ccdf=%.6f" % CCDF)
print("cdf (ref)=%.6f" % distributionRef.computeCDF(point))

# quantile
quantile = distribution.computeQuantile(0.95)
print("quantile=", repr(quantile))
print("quantile (ref)=", repr(distributionRef.computeQuantile(0.95)))
print("cdf(quantile)=%.6f" % distribution.computeCDF(quantile))
# Get 95% survival function
inverseSurvival = ot.Point(distribution.computeInverseSurvivalFunction(0.95))
print("InverseSurvival=", repr(inverseSurvival))
print(
    "Survival(inverseSurvival)=%.6f"
    % distribution.computeSurvivalFunction(inverseSurvival)
)
# Takes too much time
# print("entropy=%.6f" % distribution.computeEntropy())
# Confidence regions
if distribution.getDimension() <= 2:
    threshold = ot.Point()
    print(
        "Minimum volume interval=",
        distribution.computeMinimumVolumeInterval(0.95, threshold),
    )
    print("threshold=", threshold)
    beta = ot.Point()
    levelSet = distribution.computeMinimumVolumeLevelSet(0.95, beta)
    print("Minimum volume level set=", levelSet)
    print("beta=", beta)
    print(
        "Bilateral confidence interval=",
        distribution.computeBilateralConfidenceInterval(0.95, beta),
    )
    print("beta=", beta)
    print(
        "Unilateral confidence interval (lower tail)=",
        distribution.computeUnilateralConfidenceInterval(0.95, False, beta),
    )
    print("beta=", beta)
    print(
        "Unilateral confidence interval (upper tail)=",
        distribution.computeUnilateralConfidenceInterval(0.95, True, beta),
    )
    print("beta=", beta)

x = [1.1, 1.6, 2.2]
q = [0.1, 0.3, 0.7]
y = [[-0.5], [0.5], [1.5]]

condCDF = distribution.computeConditionalCDF(x[0], y[0])
print("cond. cdf=%.6f" % condCDF)
condCDFs = distribution.computeConditionalCDF(x, y)
print("cond. cdf (vect)=", condCDFs)
condPDF = distribution.computeConditionalPDF(x[0], y[0])
print("cond. pdf=%.6f" % condPDF)
condPDFs = distribution.computeConditionalPDF(x, y)
print("cond. pdf (vect)=", condPDFs)
condQuantile = distribution.computeConditionalQuantile(q[0], y[0])
print("cond. quantile=%.5f" % condQuantile)
condQuantiles = distribution.computeConditionalQuantile(q, y)
print("cond. quantile (vect)=", condQuantiles)
print(
    "cond. cdf(cond. quantile)=", distribution.computeConditionalCDF(condQuantiles, y)
)
pt = ot.Point([i + 1.5 for i in range(dimension)])
print(
    "sequential conditional PDF=", distribution.computeSequentialConditionalPDF(point)
)
resCDF = distribution.computeSequentialConditionalCDF(pt)
print("sequential conditional CDF(", pt, ")=", resCDF)
print(
    "sequential conditional quantile(",
    resCDF,
    ")=",
    distribution.computeSequentialConditionalQuantile(resCDF),
)

mean = distribution.getMean()
print("mean=", repr(mean))
print("mean (ref)=", repr(distributionRef.getMean()))
covariance = distribution.getCovariance()
print("covariance=", repr(covariance))
print("covariance (ref)=", repr(distributionRef.getCovariance()))
parameters = distribution.getParameter()
print("parameters=", parameters)
print("parametersDesc=", distribution.getParameterDescription())
distribution.setParameter(parameters)

ot.Log.Show(ot.Log.TRACE)
checker = ott.DistributionChecker(distribution)
checker.skipCorrelation()  # slow
checker.run()

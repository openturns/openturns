#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Instantiate one distribution object
allDistributions = [ot.Student(6.5, -0.5, 2.0)]
dim = 2
R = ot.CorrelationMatrix(dim)
mu = list()
sigma = list()
for i in range(dim):
    mu.append(i)
    sigma.append((1.0 + i) / dim)
    for j in range(i):
        R[i, j] = 1.0 / (1.0 + dim + i + j)
allDistributions.append(ot.Student(7.5, mu, sigma, R))
for distribution in allDistributions:
    dim = distribution.getDimension()
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

    # Define a point
    point = ot.Point(distribution.getDimension(), 1.0)
    print("Point= ", repr(point))

    # derivative of PDF with regards its arguments
    DDF = distribution.computeDDF(point)
    print("ddf     =", repr(DDF))

    # PDF value
    LPDF = distribution.computeLogPDF(point)
    print("log pdf=%.6f" % LPDF)
    PDF = distribution.computePDF(point)
    print("pdf     =%.6f" % PDF)

    CDF = distribution.computeCDF(point)
    print("cdf=%.6f" % CDF)
    CCDF = distribution.computeComplementaryCDF(point)
    print("ccdf=%.6f" % CCDF)

# quantile
quantile = distribution.computeQuantile(0.95)
print("quantile=", repr(quantile))
print("cdf(quantile)=%.6f" % distribution.computeCDF(quantile))
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

# Specific to this distribution
beta = point.normSquare()
densityGenerator = distribution.computeDensityGenerator(beta)
print("density generator=%.6f" % densityGenerator)
print(
    "pdf via density generator=%.6f"
    % ot.EllipticalDistribution.computePDF(distribution, point)
)
densityGeneratorDerivative = distribution.computeDensityGeneratorDerivative(beta)
print("density generator derivative     =%.6f" % densityGeneratorDerivative)
eps = 1e-5
print(
    "density generator derivative (FD)=%.6f"
    % (
        (
            distribution.computeDensityGenerator(beta + eps)
            - distribution.computeDensityGenerator(beta - eps)
        )
        / (2.0 * eps)
    )
)
densityGeneratorSecondDerivative = distribution.computeDensityGeneratorSecondDerivative(
    beta
)
print(
    "density generator second derivative     =%.6f" % densityGeneratorSecondDerivative
)
print(
    "density generator second derivative (FD)=%.6f"
    % (
        (
            distribution.computeDensityGeneratorDerivative(beta + eps)
            - distribution.computeDensityGeneratorDerivative(beta - eps)
        )
        / (2.0 * eps)
    )
)

x = 0.6
y = [0.2] * (dim - 1)
print("conditional PDF=%.6f" % distribution.computeConditionalPDF(x, y))
print("conditional CDF=%.6f" % distribution.computeConditionalCDF(x, y))
print("conditional quantile=%.6f" % distribution.computeConditionalQuantile(x, y))
pt = ot.Point([i + 1.5 for i in range(dim)])
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

ot.Log.Show(ot.Log.TRACE)
checker = ott.DistributionChecker(distribution)
checker.skipCorrelation()  # slow
checker.run()

#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# Instantiate one distribution object
mesher = ot.LevelSetMesher([25] * 2)
function = ot.SymbolicFunction(["x0", "x1"], ["10*(x0^3+x1)^2+x0^2"])
level = 0.5
domain = ot.LevelSet(function, ot.LessOrEqual(), level)
lower = [-0.75, -0.5]
upper = [0.75, 0.5]
mesh = mesher.build(domain, ot.Interval(lower, upper), False)
distribution = ot.UniformOverMesh(mesh)
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
point = [0.1] * 2
print("Point= ", point)

# Show PDF and CDF of point
eps = 1e-5
DDF = distribution.computeDDF(point)
print("ddf     =", DDF)
LPDF = distribution.computeLogPDF(point)
print("log pdf=%.5g" % LPDF)
PDF = distribution.computePDF(point)
print("pdf     =%.5g" % PDF)
CDF = distribution.computeCDF(point)
print("cdf=%.5g" % CDF)
# Too expansive for a test
if False:
    CCDF = distribution.computeComplementaryCDF(point)
    print("ccdf=", CCDF)
    Survival = distribution.computeSurvivalFunction(point)
    print("survival=", Survival)
    InverseSurvival = distribution.computeInverseSurvivalFunction(0.95)
    print("Inverse survival=", InverseSurvival)
    print(
        "Survival(inverse survival)=",
        distribution.computeSurvivalFunction(InverseSurvival),
    )
    quantile = distribution.computeQuantile(0.95)
    print("quantile=", quantile)
    print("cdf(quantile)=", distribution.computeCDF(quantile))
    # Confidence regions
    (
        interval,
        threshold,
    ) = distribution.computeMinimumVolumeIntervalWithMarginalProbability(0.95)
    print("Minimum volume interval=", interval)
    print("threshold=", threshold)
    levelSet, beta = distribution.computeMinimumVolumeLevelSetWithThreshold(0.95)
    print("Minimum volume level set=", levelSet)
    print("beta=", beta)
    (
        interval,
        beta,
    ) = distribution.computeBilateralConfidenceIntervalWithMarginalProbability(0.95)
    print("Bilateral confidence interval=", interval)
    print("beta=", beta)
    (
        interval,
        beta,
    ) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(
        0.95, False
    )
    print("Unilateral confidence interval (lower tail)=", interval)
    print("beta=", beta)
    (
        interval,
        beta,
    ) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(
        0.95, True
    )
    print("Unilateral confidence interval (upper tail)=", interval)
    print("beta=", beta)
print("entropy=%.5g" % distribution.computeEntropy())
print(
    "entropy (MC)=%.5g"
    % -distribution.computeLogPDF(distribution.getSample(1000000)).computeMean()[0]
)
mean = distribution.getMean()
print("mean=", mean)
# Too expansive for a test
if False:
    standardDeviation = distribution.getStandardDeviation()
    print("standard deviation=", standardDeviation)
    skewness = distribution.getSkewness()
    print("skewness=", skewness)
    kurtosis = distribution.getKurtosis()
    print("kurtosis=", kurtosis)
    covariance = distribution.getCovariance()
    print("covariance=", covariance)
    correlation = distribution.getCorrelation()
    print("correlation=", correlation)
    spearman = distribution.getSpearmanCorrelation()
    print("spearman=", spearman)
    kendall = distribution.getKendallTau()
    print("kendall=", kendall)
    parameters = distribution.getParametersCollection()
    print("parameters=", parameters)
    print("Standard representative=", distribution.getStandardRepresentative())

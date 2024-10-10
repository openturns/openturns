#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

all_cases = [
    ot.GeneralizedExtremeValue(2.0, 1.5, -0.15),
    ot.GeneralizedExtremeValue(2.0, 1.5, 0.0),
    ot.GeneralizedExtremeValue(2.0, 1.5, 0.15),
]
for i in range(len(all_cases)):
    distribution = all_cases[i]
    print("#" * 50)
    print("Distribution ", distribution)

    for dist in ["WeibullMax", "Frechet", "Gumbel"]:
        try:
            eval("print('conversion as ', distribution.as" + dist + "())")
        except Exception:
            pass

    # Is this distribution elliptical ?
    print("Elliptical = ", distribution.isElliptical())

    # Is this distribution continuous ?
    print("Continuous = ", distribution.isContinuous())

    # Test for realization of distribution
    oneRealization = distribution.getRealization()
    print("oneRealization=", oneRealization)

    # Define a point
    point = [1.0] * distribution.getDimension()
    print("Point= ", point)

    # Show PDF and CDF of point
    DDF = distribution.computeDDF(point)
    print("ddf     =", DDF)
    LPDF = distribution.computeLogPDF(point)
    print("log pdf=", ot.Point(1, LPDF))
    PDF = distribution.computePDF(point)
    print("pdf     =", ot.Point(1, PDF))
    CDF = distribution.computeCDF(point)
    print("cdf=", ot.Point(1, CDF))
    CCDF = distribution.computeComplementaryCDF(point)
    print("ccdf=", ot.Point(1, CCDF))
    Survival = distribution.computeSurvivalFunction(point)
    print("survival=", ot.Point(1, Survival))
    InverseSurvival = distribution.computeInverseSurvivalFunction(0.95)
    print("Inverse survival=", InverseSurvival)
    print(
        "Survival(inverse survival)=",
        ot.Point(1, distribution.computeSurvivalFunction(InverseSurvival)),
    )
    PDFgr = distribution.computePDFGradient(point)
    print("pdf gradient     =", PDFgr)
    CDFgr = distribution.computeCDFGradient(point)
    print("cdf gradient     =", CDFgr)
    quantile = distribution.computeQuantile(0.95)
    print("quantile=", quantile)
    rl = distribution.computeReturnLevel(10.0)
    print(f"return level={rl:.6f}")
    print("cdf(quantile)=", distribution.computeCDF(quantile))
    print("entropy=%.6f" % distribution.computeEntropy())
    # Confidence regions
    prob, threshold = distribution.computeMinimumVolumeIntervalWithMarginalProbability(
        0.95
    )
    print("Minimum volume interval=", prob)
    print("threshold=", threshold)
    levelSet, beta = distribution.computeMinimumVolumeLevelSetWithThreshold(0.95)
    print("Minimum volume level set=", levelSet)
    print("beta=", ot.Point(1, beta))
    (
        interval,
        beta,
    ) = distribution.computeBilateralConfidenceIntervalWithMarginalProbability(0.95)
    print("Bilateral confidence interval=", interval)
    print("beta=", ot.Point(1, beta))
    (
        interval,
        beta,
    ) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(
        0.95, False
    )
    print("Unilateral confidence interval (lower tail)=", interval)
    print("beta=", ot.Point(1, beta))
    (
        interval,
        beta,
    ) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(
        0.95, True
    )
    print("Unilateral confidence interval (upper tail)=", interval)
    print("beta=", ot.Point(1, beta))
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
    correlation = distribution.getCorrelation()
    print("correlation=", correlation)
    spearman = distribution.getSpearmanCorrelation()
    print("spearman=", spearman)
    kendall = distribution.getKendallTau()
    print("kendall=", kendall)
    parameters = distribution.getParametersCollection()
    print("parameters=", parameters)
    print("Standard representative=", distribution.getStandardRepresentative())
    print("mu=", distribution.getMu())
    print("sigma=", distribution.getSigma())
    print("xi=", distribution.getXi())
    print("Actual distribution=", distribution.getActualDistribution())
    distribution.setActualDistribution(distribution.getActualDistribution())
    print("Distribution from actual distribution=", distribution)

    ot.Log.Show(ot.Log.TRACE)
    checker = ott.DistributionChecker(distribution)
    checker.skipMinimumVolumeLevelSet()
    checker.run()

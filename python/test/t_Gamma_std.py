#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(2)

for distribution in [ot.Gamma(1.5, 2.5, -0.5), ot.Gamma(15.0, 2.5)]:
    print("Distribution ", distribution)

    # Is this distribution elliptical ?
    print("Elliptical = ", distribution.isElliptical())

    # Is this distribution continuous ?
    print("Continuous = ", distribution.isContinuous())

    # Test for realization of distribution
    oneRealization = distribution.getRealization()
    print("oneRealization=", oneRealization)

    # Define a point
    point = ot.Point(distribution.getDimension(), 1.0)
    print("Point= ", point)

    # Show PDF and CDF of point
    eps = 1e-5
    DDF = distribution.computeDDF(point)
    print("ddf     =", DDF)

    LPDF = distribution.computeLogPDF(point)
    print("log pdf= %.6g" % LPDF)
    PDF = distribution.computePDF(point)
    print("pdf     =%.6g" % PDF)

    CDF = distribution.computeCDF(point)
    print("cdf= %.6g" % CDF)
    CCDF = distribution.computeComplementaryCDF(point)
    print("ccdf= %.6g" % CCDF)
    Survival = distribution.computeSurvivalFunction(point)
    print("survival= %.6g" % Survival)
    CF = distribution.computeCharacteristicFunction(point[0])
    print("characteristic function=(%.6g, %.6g)" % (CF.real, CF.imag))
    LCF = distribution.computeLogCharacteristicFunction(point[0])
    print("log characteristic function=(%.6g, %.6g)" % (LCF.real, LCF.imag))
    PDFgr = distribution.computePDFGradient(point)
    print("pdf gradient     =", PDFgr)

    CDFgr = distribution.computeCDFGradient(point)
    print("cdf gradient     =", CDFgr)

    quantile = distribution.computeQuantile(0.95)
    print("quantile=", quantile)
    print("cdf(quantile)=", distribution.computeCDF(quantile))
    # Get 95% survival function
    inverseSurvival = ot.Point(distribution.computeInverseSurvivalFunction(0.95))
    print("InverseSurvival=", repr(inverseSurvival))
    print(
        "Survival(inverseSurvival)=%.6f"
        % distribution.computeSurvivalFunction(inverseSurvival)
    )
    print("entropy=%.6f" % distribution.computeEntropy())

    # Confidence regions
    (
        interval,
        threshold,
    ) = distribution.computeMinimumVolumeIntervalWithMarginalProbability(0.95)
    if distribution.getK() == 1.5:
        ott.assert_almost_equal(interval.getLowerBound(), [-0.49937], 1e-4, 0.0)
        ott.assert_almost_equal(interval.getUpperBound(), [1.06337], 1e-4, 0.0)
    elif distribution.getK() == 15.0:
        ott.assert_almost_equal(interval.getLowerBound(), [3.1431], 1e-4, 0.0)
        ott.assert_almost_equal(interval.getUpperBound(), [9.09027], 1e-4, 0.0)
    print("threshold=", ot.Point(1, threshold))
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

    standardDeviation = distribution.getStandardDeviation()
    print("standard deviation=", standardDeviation)
    skewness = distribution.getSkewness()
    print("skewness=", skewness)
    kurtosis = distribution.getKurtosis()
    print("kurtosis=", kurtosis)

    ot.Log.Show(ot.Log.TRACE)
    validation = ott.DistributionValidation(distribution)
    validation.run()

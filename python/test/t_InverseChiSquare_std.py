#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

allDistributions = [ot.InverseChiSquare(10.5), ot.InverseChiSquare(15.0)]
for n in range(len(allDistributions)):
    distribution = allDistributions[n]
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
        if ot.FittingTest.Kolmogorov(
            distribution.getSample(size), distribution
        ).getBinaryQualityMeasure():
            msg = "accepted"
        else:
            msg = "rejected"
        print("Kolmogorov test for the generator, sample size=", size, " is ", msg)
        size *= 10

    # Define a point
    point = ot.Point(distribution.getDimension(), 2.0 / distribution.getNu())
    print("Point= ", point)

    # Show PDF and CDF of point
    eps = 1e-5
    DDF = distribution.computeDDF(point)
    print("ddf     =", DDF)
    print(
        "ddf (FD)=%.6g"
        % (
            (
                distribution.computePDF(point + ot.Point(1, eps))
                - distribution.computePDF(point + ot.Point(1, -eps))
            )
            / (2.0 * eps)
        )
    )
    LPDF = distribution.computeLogPDF(point)
    print("log pdf= %.12g" % LPDF)
    PDF = distribution.computePDF(point)
    print("pdf     =%.6g" % PDF)
    print(
        "pdf (FD)=%.6g"
        % (
            (
                distribution.computeCDF(point + ot.Point(1, eps))
                - distribution.computeCDF(point + ot.Point(1, -eps))
            )
            / (2.0 * eps)
        )
    )
    CDF = distribution.computeCDF(point)
    print("cdf= %.12g" % CDF)
    CCDF = distribution.computeComplementaryCDF(point)
    print("ccdf= %.12g" % CCDF)
    Survival = distribution.computeSurvivalFunction(point)
    print("survival= %.12g" % Survival)
    CF = distribution.computeCharacteristicFunction(point[0])
    print("characteristic function=(%.6g, %.6g)" % (CF.real, CF.imag))
    LCF = distribution.computeLogCharacteristicFunction(point[0])
    print("log characteristic function=(%.6g, %.6g)" % (LCF.real, LCF.imag))
    PDFgr = distribution.computePDFGradient(point)
    print("pdf gradient     =", PDFgr)
    PDFgrFD = ot.Point(1)
    PDFgrFD[0] = (
        ot.InverseChiSquare(distribution.getNu() + eps).computePDF(point)
        - ot.InverseChiSquare(distribution.getNu() - eps).computePDF(point)
    ) / (2.0 * eps)
    print("pdf gradient (FD)=", PDFgrFD)
    CDFgr = distribution.computeCDFGradient(point)
    print("cdf gradient     =", CDFgr)
    CDFgrFD = ot.Point(1)
    CDFgrFD[0] = (
        ot.InverseChiSquare(distribution.getNu() + eps).computeCDF(point)
        - ot.InverseChiSquare(distribution.getNu() - eps).computeCDF(point)
    ) / (2.0 * eps)
    print("cdf gradient (FD)=", CDFgrFD)
    quantile = distribution.computeQuantile(0.95)
    print("quantile=", quantile)
    print("cdf(quantile)= %.2f" % distribution.computeCDF(quantile))
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
    print("Minimum volume interval=", interval)
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

    # computeProba test with bound far away
    p = distribution.computeProbability(ot.Interval(-ot.SpecFunc.MaxScalar, ot.SpecFunc.MaxScalar))
    ott.assert_almost_equal(p, 1.0)

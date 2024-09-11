#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

coll = [
    ot.Trapezoidal(1.0, 1.2, 3.0, 14.0),
    ot.Trapezoidal(1.0, 1.0, 3.0, 14.0),
    ot.Trapezoidal(1.0, 1.2, 1.2, 14.0),
    ot.Trapezoidal(1.0, 1.0, 1.0, 14.0),
    ot.Trapezoidal(1.0, 1.2, 14.0, 14.0),
    ot.Trapezoidal(1.0, 1.0, 14.0, 14.0),
    ot.Trapezoidal(1.0, 14.0, 14.0, 14.0),
]

# Instantiate one distribution object
for distribution in coll:
    print("Distribution ", repr(distribution))
    print("Distribution ", distribution)

    # Get mean and covariance
    print("Mean= ", repr(distribution.getMean()))
    print("Covariance= ", repr(distribution.getCovariance()))

    # Is this distribution elliptical ?
    print("Elliptical = ", distribution.isElliptical())

    # Test for realization of distribution
    oneRealization = distribution.getRealization()
    print("oneRealization=", repr(oneRealization))

    # Define a point
    point = ot.Point(distribution.getDimension(), 1.1)
    print("Point= ", repr(point))

    # derivative of PDF with regards its arguments
    DDF = distribution.computeDDF(point)
    print("ddf     =", repr(DDF))

    # PDF value
    LPDF = distribution.computeLogPDF(point)
    print("log pdf=%.6f" % LPDF)
    PDF = distribution.computePDF(point)
    print("pdf     =%.6f" % PDF)

    # derivative of the PDF with regards the parameters of the distribution
    CDF = distribution.computeCDF(point)
    print("cdf=%.6f" % CDF)
    CCDF = distribution.computeComplementaryCDF(point)
    print("ccdf=%.6f" % CCDF)
    CF = distribution.computeCharacteristicFunction(point[0])
    print("characteristic function= (%.12g%+.12gj)" % (CF.real, CF.imag))
    try:
        PDFgr = distribution.computePDFGradient(point)
        print("pdf gradient     =", repr(PDFgr))

        # derivative of the logPDF with regards the parameters of the distribution
        logPDFgr = distribution.computeLogPDFGradient(point)
        print("log-pdf gradient     =", repr(logPDFgr))

        # derivative of the PDF with regards the parameters of the distribution
        CDFgr = distribution.computeCDFGradient(point)
        print("cdf gradient     =", repr(CDFgr))
    except Exception:
        pass

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

    roughness = distribution.getRoughness()
    print("roughness=", ot.Point(1, roughness))

    # check only non-degenerate cases
    if len(set(distribution.getParameter())) == 4:
        ot.Log.Show(ot.Log.TRACE)
        checker = ott.DistributionChecker(distribution)
        checker.run()

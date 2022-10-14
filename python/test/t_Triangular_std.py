#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


coll = [
    ot.Triangular(-0.5, 1.5, 2.5),
    ot.Triangular(-0.5, -0.5, 2.5),
    ot.Triangular(-0.5, 2.5, 2.5),
]
# Instantiate one distribution object
for distribution in coll:
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

    # Define a point, left part of the support
    point = ot.Point(distribution.getDimension(), 1.0)
    print("Point= ", repr(point))

    # Show PDF and CDF of point
    eps = 1e-5

    # derivative of PDF with regards its arguments
    DDF = distribution.computeDDF(point)
    print("ddf     =", repr(DDF))
    # by the finite difference technique
    print(
        "ddf (FD)=",
        repr(
            ot.Point(
                1,
                (
                    distribution.computePDF(point + ot.Point(1, eps))
                    - distribution.computePDF(point + ot.Point(1, -eps))
                )
                / (2.0 * eps),
            )
        ),
    )

    # PDF value
    LPDF = distribution.computeLogPDF(point)
    print("log pdf=%.6f" % LPDF)
    PDF = distribution.computePDF(point)
    print("pdf     =%.6f" % PDF)
    # by the finite difference technique from CDF
    print(
        "pdf (FD)=%.6f"
        % (
            (
                distribution.computeCDF(point + ot.Point(1, eps))
                - distribution.computeCDF(point + ot.Point(1, -eps))
            )
            / (2.0 * eps)
        )
    )

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
        # by the finite difference technique
        PDFgrFD = ot.Point(3)
        PDFgrFD[0] = (
            ot.Triangular(
                distribution.getA() + eps, distribution.getM(), distribution.getB()
            ).computePDF(point)
            - ot.Triangular(
                distribution.getA() - eps, distribution.getM(), distribution.getB()
            ).computePDF(point)
        ) / (2.0 * eps)
        PDFgrFD[1] = (
            ot.Triangular(
                distribution.getA(), distribution.getM() + eps, distribution.getB()
            ).computePDF(point)
            - ot.Triangular(
                distribution.getA(), distribution.getM() - eps, distribution.getB()
            ).computePDF(point)
        ) / (2.0 * eps)
        PDFgrFD[2] = (
            ot.Triangular(
                distribution.getA(), distribution.getM(), distribution.getB() + eps
            ).computePDF(point)
            - ot.Triangular(
                distribution.getA(), distribution.getM(), distribution.getB() - eps
            ).computePDF(point)
        ) / (2.0 * eps)
        print("pdf gradient (FD)=", repr(PDFgrFD))

        # derivative of the PDF with regards the parameters of the distribution
        CDFgr = distribution.computeCDFGradient(point)
        print("cdf gradient     =", repr(CDFgr))
        CDFgrFD = ot.Point(3)
        CDFgrFD[0] = (
            ot.Triangular(
                distribution.getA() + eps, distribution.getM(), distribution.getB()
            ).computeCDF(point)
            - ot.Triangular(
                distribution.getA() - eps, distribution.getM(), distribution.getB()
            ).computeCDF(point)
        ) / (2.0 * eps)
        CDFgrFD[1] = (
            ot.Triangular(
                distribution.getA(), distribution.getM() + eps, distribution.getB()
            ).computeCDF(point)
            - ot.Triangular(
                distribution.getA(), distribution.getM() - eps, distribution.getB()
            ).computeCDF(point)
        ) / (2.0 * eps)
        CDFgrFD[2] = (
            ot.Triangular(
                distribution.getA(), distribution.getM(), distribution.getB() + eps
            ).computeCDF(point)
            - ot.Triangular(
                distribution.getA(), distribution.getM(), distribution.getB() - eps
            ).computeCDF(point)
        ) / (2.0 * eps)
        print("cdf gradient (FD)=", repr(CDFgrFD))
    except Exception:
        pass

    # quantile
    quantile = distribution.computeQuantile(0.25)
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

    # Define a point, right part of the support
    point = ot.Point(distribution.getDimension(), 2.0)
    print("Point= ", repr(point))

    # derivative of PDF with regards its arguments
    DDF = distribution.computeDDF(point)
    print("ddf     =", repr(DDF))
    # by the finite difference technique
    print(
        "ddf (FD)=",
        repr(
            ot.Point(
                1,
                (
                    distribution.computePDF(point + ot.Point(1, eps))
                    - distribution.computePDF(point + ot.Point(1, -eps))
                )
                / (2.0 * eps),
            )
        ),
    )

    # PDF value
    LPDF = distribution.computeLogPDF(point)
    print("log pdf=%.6f" % LPDF)
    PDF = distribution.computePDF(point)
    print("pdf     =%.6f" % PDF)
    # by the finite difference technique from CDF
    print(
        "pdf (FD)=%.6f"
        % (
            (
                distribution.computeCDF(point + ot.Point(1, eps))
                - distribution.computeCDF(point + ot.Point(1, -eps))
            )
            / (2.0 * eps)
        )
    )

    # derivative of the PDF with regards the parameters of the distribution
    CDF = distribution.computeCDF(point)
    print("cdf=%.6f" % CDF)
    CCDF = distribution.computeComplementaryCDF(point)
    print("ccdf=%.6f" % CCDF)
    try:
        PDFgr = distribution.computePDFGradient(point)
        print("pdf gradient     =", repr(PDFgr))
        # by the finite difference technique
        PDFgrFD[0] = (
            ot.Triangular(
                distribution.getA() + eps, distribution.getM(), distribution.getB()
            ).computePDF(point)
            - ot.Triangular(
                distribution.getA() - eps, distribution.getM(), distribution.getB()
            ).computePDF(point)
        ) / (2.0 * eps)
        PDFgrFD[1] = (
            ot.Triangular(
                distribution.getA(), distribution.getM() + eps, distribution.getB()
            ).computePDF(point)
            - ot.Triangular(
                distribution.getA(), distribution.getM() - eps, distribution.getB()
            ).computePDF(point)
        ) / (2.0 * eps)
        PDFgrFD[2] = (
            ot.Triangular(
                distribution.getA(), distribution.getM(), distribution.getB() + eps
            ).computePDF(point)
            - ot.Triangular(
                distribution.getA(), distribution.getM(), distribution.getB() - eps
            ).computePDF(point)
        ) / (2.0 * eps)
        print("pdf gradient (FD)=", repr(PDFgrFD))

        # derivative of the PDF with regards the parameters of the distribution
        CDFgr = distribution.computeCDFGradient(point)
        print("cdf gradient     =", repr(CDFgr))
        CDFgrFD = ot.Point(3)
        CDFgrFD[0] = (
            ot.Triangular(
                distribution.getA() + eps, distribution.getM(), distribution.getB()
            ).computeCDF(point)
            - ot.Triangular(
                distribution.getA() - eps, distribution.getM(), distribution.getB()
            ).computeCDF(point)
        ) / (2.0 * eps)
        CDFgrFD[1] = (
            ot.Triangular(
                distribution.getA(), distribution.getM() + eps, distribution.getB()
            ).computeCDF(point)
            - ot.Triangular(
                distribution.getA(), distribution.getM() - eps, distribution.getB()
            ).computeCDF(point)
        ) / (2.0 * eps)
        CDFgrFD[2] = (
            ot.Triangular(
                distribution.getA(), distribution.getM(), distribution.getB() + eps
            ).computeCDF(point)
            - ot.Triangular(
                distribution.getA(), distribution.getM(), distribution.getB() - eps
            ).computeCDF(point)
        ) / (2.0 * eps)
        print("cdf gradient (FD)=", repr(CDFgrFD))
    except Exception:
        pass

    # quantile
    quantile = distribution.computeQuantile(0.95)
    print("quantile=", repr(quantile))
    print("cdf(quantile)=%.6f" % distribution.computeCDF(quantile))
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

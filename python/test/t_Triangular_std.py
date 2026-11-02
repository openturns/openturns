#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()


coll = [
    ot.Triangular(-0.5, 1.5, 2.5),
    ot.Triangular(-0.5, -0.5, 2.5),
    ot.Triangular(-0.5, 2.5, 2.5),
]
expectedRealization = [1.44403, 0.606457, 1.58512]
expectedDDF = [0.333333, -0.222222, 0.222222]
expectedLPDF = [-0.693147, -1.098612, -1.098612]
expectedPDF = [0.5, 0.333333, 0.333333]
expectedCDF = [0.375, 0.75, 0.25]
expectedCCDF = [0.625, 0.25, 0.75]
expectedCF = [
    (0.312305424736, 0.758322070069),
    (0.692667287389, 0.345522222881),
    (0.0259311677499, 0.773628562078),
]
expectedQuantile25 = [0.724745, -0.0980762, 1.0]
expectedInverseSurvival = [0.0477226, -0.424038, 0.17082]
expectedEntropy = [0.905465, 0.905465, 0.905465]
expectedMVLower = [-0.0527864, -0.5, 0.17082]
expectedMVUpper = [2.27639, 1.82918, 2.5]
expectedMVBeta = [0.149071, 0.149071, 0.149071]
expectedBilateralLower = [-0.112702, -0.462263, -0.0256584]
expectedBilateralUpper = [2.22614, 2.02566, 2.46226]
expectedUnilateralLower = [-0.5, -0.5, -0.5]
expectedUnilateralUpper = [2.1127, 1.82918, 2.42404]
expectedSurvivalLower = [0.0477226, -0.424038, 0.17082]
expectedSurvivalUpper = [2.5, 2.5, 2.5]
expectedDDF2 = [-0.666667, -0.222222, 0.222222]
expectedLPDF2 = [-1.098612, -2.197225, -0.587787]
expectedPDF2 = [0.333333, 0.111111, 0.555556]
expectedCDF2 = [0.916667, 0.972222, 0.694444]
expectedCCDF2 = [0.083333, 0.027778, 0.305556]
expectedQuantile95 = [2.1127, 1.82918, 2.42404]
expectedMean = [1.16667, 0.5, 1.5]
expectedStd = [0.62361, 0.707107, 0.707107]
expectedSkewness = [-0.305441, 0.565685, -0.565685]
expectedKurtosis = [2.4, 2.4, 2.4]
expectedCovariance = [0.388889, 0.5, 0.5]
expectedParams = [(-0.5, 1.5, 2.5), (-0.5, -0.5, 2.5), (-0.5, 2.5, 2.5)]
# Instantiate one distribution object
for i, distribution in enumerate(coll):
    assert distribution.getClassName() == "Triangular"

    # Is this distribution elliptical ?
    assert not distribution.isElliptical()

    # Is this distribution continuous ?
    assert distribution.isContinuous()

    # Test for realization of distribution
    oneRealization = distribution.getRealization()
    ott.assert_almost_equal(oneRealization, [expectedRealization[i]], 1e-5, 0.0)

    # Define a point, left part of the support
    point = ot.Point(distribution.getDimension(), 1.0)
    ott.assert_almost_equal(point, [1.0], 0.0, 0.0)

    # derivative of PDF with regards its arguments
    DDF = distribution.computeDDF(point)
    ott.assert_almost_equal(DDF, [expectedDDF[i]], 1e-5, 0.0)

    # PDF value
    LPDF = distribution.computeLogPDF(point)
    ott.assert_almost_equal(LPDF, expectedLPDF[i], 1e-6, 1e-6)
    PDF = distribution.computePDF(point)
    ott.assert_almost_equal(PDF, expectedPDF[i], 1e-6, 1e-6)

    # derivative of the PDF with regards the parameters of the distribution
    CDF = distribution.computeCDF(point)
    ott.assert_almost_equal(CDF, expectedCDF[i], 1e-6, 1e-6)
    CCDF = distribution.computeComplementaryCDF(point)
    ott.assert_almost_equal(CCDF, expectedCCDF[i], 1e-6, 1e-6)
    CF = distribution.computeCharacteristicFunction(point[0])
    ott.assert_almost_equal([CF.real, CF.imag], list(expectedCF[i]), 1e-9, 0.0)
    if i == 0:
        # gradients throw on degenerate configurations
        PDFgr = distribution.computePDFGradient(point)
        ott.assert_almost_equal(PDFgr, [0.0833333, -0.25, -0.166667], 1e-5, 0.0)

        # derivative of the PDF with regards the parameters of the distribution
        CDFgr = distribution.computeCDFGradient(point)
        ott.assert_almost_equal(CDFgr, [-0.1875, -0.1875, -0.125], 1e-5, 0.0)
    else:
        with ott.assert_raises(Exception):
            distribution.computePDFGradient(point)

    # quantile
    quantile = distribution.computeQuantile(0.25)
    ott.assert_almost_equal(quantile, [expectedQuantile25[i]], 1e-5, 0.0)
    ott.assert_almost_equal(distribution.computeCDF(quantile), 0.25, 1e-6, 1e-6)
    # Get 95% survival function
    inverseSurvival = ot.Point(distribution.computeInverseSurvivalFunction(0.95))
    ott.assert_almost_equal(inverseSurvival, [expectedInverseSurvival[i]], 1e-5, 0.0)
    ott.assert_almost_equal(
        distribution.computeSurvivalFunction(inverseSurvival), 0.95, 1e-6, 1e-6
    )
    ott.assert_almost_equal(distribution.computeEntropy(), expectedEntropy[i], 1e-6, 1e-6)

    # Confidence regions
    interval, threshold = distribution.computeMinimumVolumeIntervalWithMarginalProbability(
        0.95
    )
    ott.assert_almost_equal(interval.getLowerBound(), [expectedMVLower[i]], 1e-5, 0.0)
    ott.assert_almost_equal(interval.getUpperBound(), [expectedMVUpper[i]], 1e-5, 0.0)
    ott.assert_almost_equal(threshold, 0.95, 1e-5, 0.0)
    levelSet, beta = distribution.computeMinimumVolumeLevelSetWithThreshold(0.95)
    ott.assert_almost_equal(beta, expectedMVBeta[i], 1e-5, 0.0)
    interval, beta = distribution.computeBilateralConfidenceIntervalWithMarginalProbability(
        0.95
    )
    ott.assert_almost_equal(interval.getLowerBound(), [expectedBilateralLower[i]], 1e-5, 0.0)
    ott.assert_almost_equal(interval.getUpperBound(), [expectedBilateralUpper[i]], 1e-5, 0.0)
    ott.assert_almost_equal(beta, 0.95, 1e-5, 0.0)
    (
        interval,
        beta,
    ) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(
        0.95, False
    )
    ott.assert_almost_equal(interval.getLowerBound(), [expectedUnilateralLower[i]], 1e-5, 0.0)
    ott.assert_almost_equal(interval.getUpperBound(), [expectedUnilateralUpper[i]], 1e-5, 0.0)
    ott.assert_almost_equal(beta, 0.95, 1e-5, 0.0)
    (
        interval,
        beta,
    ) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(
        0.95, True
    )
    ott.assert_almost_equal(interval.getLowerBound(), [expectedSurvivalLower[i]], 1e-5, 0.0)
    ott.assert_almost_equal(interval.getUpperBound(), [expectedSurvivalUpper[i]], 1e-5, 0.0)
    ott.assert_almost_equal(beta, 0.95, 1e-5, 0.0)

    # Define a point, right part of the support
    point = ot.Point(distribution.getDimension(), 2.0)
    ott.assert_almost_equal(point, [2.0], 0.0, 0.0)

    # derivative of PDF with regards its arguments
    DDF = distribution.computeDDF(point)
    ott.assert_almost_equal(DDF, [expectedDDF2[i]], 1e-5, 0.0)

    # PDF value
    LPDF = distribution.computeLogPDF(point)
    ott.assert_almost_equal(LPDF, expectedLPDF2[i], 1e-6, 1e-6)
    PDF = distribution.computePDF(point)
    ott.assert_almost_equal(PDF, expectedPDF2[i], 1e-6, 1e-6)

    # derivative of the PDF with regards the parameters of the distribution
    CDF = distribution.computeCDF(point)
    ott.assert_almost_equal(CDF, expectedCDF2[i], 1e-6, 1e-6)
    CCDF = distribution.computeComplementaryCDF(point)
    ott.assert_almost_equal(CCDF, expectedCCDF2[i], 1e-6, 1e-6)
    if i == 0:
        PDFgr = distribution.computePDFGradient(point)
        ott.assert_almost_equal(PDFgr, [0.111111, 0.333333, 0.222222], 1e-5, 0.0)

        # derivative of the PDF with regards the parameters of the distribution
        CDFgr = distribution.computeCDFGradient(point)
        ott.assert_almost_equal(CDFgr, [-0.0277778, -0.0833333, -0.222222], 1e-5, 0.0)
    else:
        with ott.assert_raises(Exception):
            distribution.computePDFGradient(point)

    # quantile
    quantile = distribution.computeQuantile(0.95)
    ott.assert_almost_equal(quantile, [expectedQuantile95[i]], 1e-5, 0.0)
    ott.assert_almost_equal(distribution.computeCDF(quantile), 0.95, 1e-6, 1e-6)
    mean = distribution.getMean()
    ott.assert_almost_equal(mean, [expectedMean[i]], 1e-5, 0.0)
    standardDeviation = distribution.getStandardDeviation()
    ott.assert_almost_equal(standardDeviation, [expectedStd[i]], 1e-5, 0.0)
    skewness = distribution.getSkewness()
    ott.assert_almost_equal(skewness, [expectedSkewness[i]], 1e-5, 0.0)
    kurtosis = distribution.getKurtosis()
    ott.assert_almost_equal(kurtosis, [expectedKurtosis[i]], 1e-5, 0.0)
    covariance = distribution.getCovariance()
    ott.assert_almost_equal(
        covariance, ot.CovarianceMatrix(1, [expectedCovariance[i]]), 1e-5, 0.0
    )
    parameters = distribution.getParametersCollection()
    assert len(parameters) == 1
    ott.assert_almost_equal(parameters[0], list(expectedParams[i]), 0.0, 0.0)

    # check only non-degenerate cases
    if len(set(distribution.getParameter())) == 3:
        ot.Log.Show(ot.Log.TRACE)
        validation = ott.DistributionValidation(distribution)
        validation.run()

# Test PDF and DDF at support boundaries for all cases (incl. degenerate)
for a, m, b in [(-0.5, 1.5, 2.5), (-0.5, -0.5, 2.5), (-0.5, 2.5, 2.5)]:
    dist = ot.Triangular(a, m, b)
    # PDF and DDF at left boundary: right-sided values, nonzero for m == a
    if m == a:
        ott.assert_almost_equal(dist.computePDF(a), 2.0 / (b - a), 1e-12, 0.0)
        ott.assert_almost_equal(dist.computeDDF(a), -2.0 / (b - a) / (b - a), 1e-12, 0.0)
    else:
        ott.assert_almost_equal(dist.computePDF(a), 0.0)
        ott.assert_almost_equal(dist.computeDDF(a), 2.0 / (b - a) / (m - a), 1e-12, 0.0)
    # PDF and DDF at right boundary: left-sided values, nonzero for m == b
    if m == b:
        ott.assert_almost_equal(dist.computePDF(b), 2.0 / (b - a), 1e-12, 0.0)
        ott.assert_almost_equal(dist.computeDDF(b), 2.0 / (b - a) / (b - a), 1e-12, 0.0)
    else:
        ott.assert_almost_equal(dist.computePDF(b), 0.0)

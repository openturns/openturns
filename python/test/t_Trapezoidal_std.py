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
expectedElliptical = [False, False, False, False, False, True, False]
expectedRealization = [6.23752, 5.89233, 4.72556, 1.36614, 5.84072, 10.6537, 13.4634]
expectedDDF = [0.675676, 0.0, 0.769231, -0.0118343, 0.387597, 0.0, 0.0118343]
expectedLPDF = [-2.694627, -2.014903, -2.564949, -1.879524, -3.250374, -2.564949, -6.739337]
expectedPDF = [0.067568, 0.133333, 0.076923, 0.152663, 0.038760, 0.076923, 0.001183]
expectedCDF = [0.003378, 0.013333, 0.003846, 0.015325, 0.001938, 0.007692, 0.000059]
expectedCCDF = [0.996622, 0.986667, 0.996154, 0.984675, 0.998062, 0.992308, 0.999941]
expectedCF = [
    (-0.115059431951, 0.0386019620712),
    (-0.108371008362, 0.0503646282831),
    (-0.1186605461, 0.0558853713745),
    (-0.110887732511, 0.0691917799374),
    (-0.0444405891758, 0.0919845543507),
    (-0.041125105127, 0.0983600726093),
    (0.0286375222568, 0.127528365281),
]
expectedQuantile = [11.1469, 11.1277, 11.1156, 11.0931, 13.355, 13.35, 13.6708]
expectedInverseSurvival = [1.47, 1.375, 1.42701, 1.32917, 1.745, 1.65, 3.90689]
expectedEntropy = [2.379858, 2.381570, 2.371802, 2.371802, 2.561103, 2.564949, 2.371802]
expectedMVLower = [1.05141, 1.0, 1.04472, 1.0, 1.3725, 1.325, 3.90689]
expectedMVUpper = [11.1725, 11.1277, 11.1378, 11.0931, 13.6275, 13.675, 14.0]
expectedMVBeta = [0.0347356, 0.0348155, 0.034401, 0.034401, 0.0775194, 0.0769231, 0.034401]
expectedBilateralLower = [1.285, 1.1875, 1.26265, 1.16353, 1.4225, 1.325, 3.05548]
expectedBilateralUpper = [11.9826, 11.969, 11.9604, 11.9445, 13.6775, 13.675, 13.8365]
expectedUnilateralLower = [1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0]
expectedUnilateralUpper = [11.1469, 11.1277, 11.1156, 11.0931, 13.355, 13.35, 13.6708]
expectedSurvivalLower = [1.47, 1.375, 1.42701, 1.32917, 1.745, 1.65, 3.90689]
expectedMean = [5.48108, 5.42222, 5.4, 5.33333, 7.54987, 7.5, 9.66667]
expectedStd = [3.02371, 3.04586, 3.04083, 3.06413, 3.72413, 3.75278, 3.06413]
expectedSkewness = [0.548305, 0.546649, 0.565227, 0.565685, -0.000208124, 0.0, -0.565685]
expectedKurtosis = [2.39403, 2.39301, 2.4, 2.4, 1.80029, 1.8, 2.4]
expectedCovariance = [9.1428, 9.27728, 9.24667, 9.38889, 13.8692, 14.0833, 9.38889]
expectedRoughness = [0.101047, 0.100741, 0.102564, 0.102564, 0.0773191, 0.0769231, 0.102564]

# Instantiate one distribution object
for i, distribution in enumerate(coll):
    assert distribution.getClassName() == "Trapezoidal"

    # Get mean and covariance
    ott.assert_almost_equal(distribution.getMean(), [expectedMean[i]], 1e-5, 0.0)
    ott.assert_almost_equal(
        distribution.getCovariance(), ot.CovarianceMatrix(1, [expectedCovariance[i]]), 1e-5, 0.0
    )

    # Is this distribution elliptical ?
    assert distribution.isElliptical() == expectedElliptical[i]

    # Test for realization of distribution
    oneRealization = distribution.getRealization()
    ott.assert_almost_equal(oneRealization, [expectedRealization[i]], 1e-5, 0.0)

    # Define a point
    point = ot.Point(distribution.getDimension(), 1.1)
    ott.assert_almost_equal(point, [1.1], 0.0, 0.0)

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
        ott.assert_almost_equal(PDFgr, [-0.333272, -0.333272, -0.00456538, -0.00456538], 1e-5, 0.0)

        # derivative of the logPDF with regards the parameters of the distribution
        logPDFgr = distribution.computeLogPDFGradient(point)
        ott.assert_almost_equal(logPDFgr, [-4.93243, -4.93243, -0.0675676, -0.0675676], 1e-5, 0.0)

        # derivative of the PDF with regards the parameters of the distribution
        CDFgr = distribution.computeCDFGradient(point)
        ott.assert_almost_equal(CDFgr, [-0.0504474, -0.0166636, -0.000228269, -0.000228269], 1e-5, 0.0)
    else:
        with ott.assert_raises(Exception):
            distribution.computePDFGradient(point)

    # quantile
    quantile = distribution.computeQuantile(0.95)
    ott.assert_almost_equal(quantile, [expectedQuantile[i]], 1e-5, 0.0)
    ott.assert_almost_equal(distribution.computeCDF(quantile), 0.95, 1e-6, 1e-6)
    # Get 95% survival function
    inverseSurvival = ot.Point(distribution.computeInverseSurvivalFunction(0.95))
    ott.assert_almost_equal(inverseSurvival, [expectedSurvivalLower[i]], 1e-5, 0.0)
    ott.assert_almost_equal(
        distribution.computeSurvivalFunction(inverseSurvival), 0.95, 1e-6, 1e-6
    )
    ott.assert_almost_equal(distribution.computeEntropy(), expectedEntropy[i], 1e-6, 1e-6)

    # Confidence regions
    (
        interval,
        threshold,
    ) = distribution.computeMinimumVolumeIntervalWithMarginalProbability(0.95)
    ott.assert_almost_equal(interval.getLowerBound(), [expectedMVLower[i]], 1e-5, 0.0)
    ott.assert_almost_equal(interval.getUpperBound(), [expectedMVUpper[i]], 1e-5, 0.0)
    ott.assert_almost_equal(threshold, 0.95, 1e-5, 0.0)
    levelSet, beta = distribution.computeMinimumVolumeLevelSetWithThreshold(0.95)
    ott.assert_almost_equal(beta, expectedMVBeta[i], 1e-5, 0.0)
    (
        interval,
        beta,
    ) = distribution.computeBilateralConfidenceIntervalWithMarginalProbability(0.95)
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
    ott.assert_almost_equal(interval.getUpperBound(), [14.0], 1e-5, 0.0)
    ott.assert_almost_equal(beta, 0.95, 1e-5, 0.0)

    mean = distribution.getMean()
    ott.assert_almost_equal(mean, [expectedMean[i]], 1e-5, 0.0)
    standardDeviation = distribution.getStandardDeviation()
    ott.assert_almost_equal(standardDeviation, [expectedStd[i]], 1e-5, 0.0)
    skewness = distribution.getSkewness()
    if i == 5:
        ott.assert_almost_equal(skewness, [0.0], 0.0, 0.0)
    else:
        ott.assert_almost_equal(skewness, [expectedSkewness[i]], 1e-5, 0.0)
    kurtosis = distribution.getKurtosis()
    ott.assert_almost_equal(kurtosis, [expectedKurtosis[i]], 1e-5, 0.0)
    covariance = distribution.getCovariance()
    ott.assert_almost_equal(
        covariance, ot.CovarianceMatrix(1, [expectedCovariance[i]]), 1e-5, 0.0
    )
    parameters = distribution.getParametersCollection()
    assert len(parameters) == 1
    ott.assert_almost_equal(parameters[0], list(coll[i].getParameter()), 0.0, 0.0)
    assert distribution.getStandardRepresentative().getImplementation().getClassName() == "Trapezoidal"

    roughness = distribution.getRoughness()
    ott.assert_almost_equal(roughness, expectedRoughness[i], 1e-5, 0.0)

    # check only non-degenerate cases
    if len(set(distribution.getParameter())) == 4:
        ot.Log.Show(ot.Log.TRACE)
        validation = ott.DistributionValidation(distribution)
        validation.run()

# Density at the support edges in degenerate cases: plateau height
ott.assert_almost_equal(ot.Trapezoidal(0.0, 0.0, 1.0, 2.0).computePDF(0.0), 2.0 / 3.0, 1e-12, 0.0)
ott.assert_almost_equal(ot.Trapezoidal(0.0, 1.0, 2.0, 2.0).computePDF(2.0), 2.0 / 3.0, 1e-12, 0.0)

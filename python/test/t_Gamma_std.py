#! /usr/bin/env python

import math

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(2)

for distribution in [ot.Gamma(1.5, 2.5, -0.5), ot.Gamma(15.0, 2.5)]:
    assert distribution.getClassName() == "Gamma"

    # Is this distribution elliptical ?
    assert not distribution.isElliptical()

    # Is this distribution continuous ?
    assert distribution.isContinuous()

    # Test for realization of distribution
    oneRealization = distribution.getRealization()
    if distribution.getK() == 1.5:
        ott.assert_almost_equal(oneRealization, [0.0406965], 1e-5, 0.0)
    else:
        ott.assert_almost_equal(oneRealization, [5.84498], 1e-5, 0.0)

    # Define a point
    point = ot.Point(distribution.getDimension(), 1.0)
    ott.assert_almost_equal(point, [1.0], 0.0, 0.0)

    # Show PDF and CDF of point
    eps = 1e-5
    DDF = distribution.computeDDF(point)
    if distribution.getK() == 1.5:
        ott.assert_almost_equal(DDF, [-0.278355], 1e-5, 0.0)
    else:
        ott.assert_almost_equal(DDF, [1.00845e-05], 1e-5, 0.0)

    LPDF = distribution.computeLogPDF(point)
    if distribution.getK() == 1.5:
        ott.assert_almost_equal(LPDF, -2.05205, 1e-5, 0.0)
    else:
        ott.assert_almost_equal(LPDF, -13.9469, 1e-5, 0.0)
    PDF = distribution.computePDF(point)
    if distribution.getK() == 1.5:
        ott.assert_almost_equal(PDF, 0.128471, 1e-5, 0.0)
    else:
        ott.assert_almost_equal(PDF, 8.76911e-07, 1e-5, 0.0)

    CDF = distribution.computeCDF(point)
    if distribution.getK() == 1.5:
        ott.assert_almost_equal(CDF, 0.942442, 1e-5, 0.0)
    else:
        ott.assert_almost_equal(CDF, 6.91531e-08, 1e-5, 0.0)
    CCDF = distribution.computeComplementaryCDF(point)
    if distribution.getK() == 1.5:
        ott.assert_almost_equal(CCDF, 0.0575585, 1e-5, 0.0)
    else:
        ott.assert_almost_equal(CCDF, 1.0, 1e-5, 0.0)
    Survival = distribution.computeSurvivalFunction(point)
    if distribution.getK() == 1.5:
        ott.assert_almost_equal(Survival, 0.0575585, 1e-5, 0.0)
    else:
        ott.assert_almost_equal(Survival, 1.0, 1e-5, 0.0)
    CF = distribution.computeCharacteristicFunction(point[0])
    if distribution.getK() == 1.5:
        ott.assert_almost_equal([CF.real, CF.imag], [0.892418, 0.0632527], 1e-5, 0.0)
    else:
        ott.assert_almost_equal([CF.real, CF.imag], [0.275588, -0.178824], 1e-5, 0.0)
    LCF = distribution.computeLogCharacteristicFunction(point[0])
    if distribution.getK() == 1.5:
        ott.assert_almost_equal([LCF.real, LCF.imag], [-0.111315, 0.0707596], 1e-5, 0.0)
    else:
        ott.assert_almost_equal([LCF.real, LCF.imag], [-1.11315, 5.7076], 1e-5, 0.0)
    PDFgr = distribution.computePDFGradient(point)
    if distribution.getK() == 1.5:
        ott.assert_almost_equal(PDFgr, [0.16512, -0.115624, 0.278355], 1e-5, 0.0)
    else:
        ott.assert_almost_equal(PDFgr, [-1.54166e-06, 4.38456e-06, -1.00845e-05], 1e-5, 0.0)

    CDFgr = distribution.computeCDFGradient(point)
    if distribution.getK() == 1.5:
        ott.assert_almost_equal(CDFgr, [-0.0875625, 0.0770828, -0.128471], 1e-5, 0.0)
    else:
        ott.assert_almost_equal(CDFgr, [-1.26959e-07, 3.50764e-07, -8.76911e-07], 1e-5, 0.0)

    quantile = distribution.computeQuantile(0.95)
    if distribution.getK() == 1.5:
        ott.assert_almost_equal(quantile, [1.06295], 1e-5, 0.0)
    else:
        ott.assert_almost_equal(quantile, [8.75459], 1e-5, 0.0)
    ott.assert_almost_equal(distribution.computeCDF(quantile), 0.95, 1e-9, 0.0)
    # Get 95% survival function
    inverseSurvival = ot.Point(distribution.computeInverseSurvivalFunction(0.95))
    if distribution.getK() == 1.5:
        ott.assert_almost_equal(inverseSurvival, [-0.429631], 1e-5, 0.0)
    else:
        ott.assert_almost_equal(inverseSurvival, [3.69853], 1e-5, 0.0)
    ott.assert_almost_equal(
        distribution.computeSurvivalFunction(inverseSurvival), 0.95, 1e-6, 1e-6
    )
    if distribution.getK() == 1.5:
        ott.assert_almost_equal(distribution.computeEntropy(), 0.444682, 1e-6, 1e-6)
    else:
        ott.assert_almost_equal(distribution.computeEntropy(), 1.834077, 1e-6, 1e-6)

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
    ott.assert_almost_equal(threshold, 0.95, 1e-5, 0.0)
    levelSet, beta = distribution.computeMinimumVolumeLevelSetWithThreshold(0.95)
    if distribution.getK() == 1.5:
        ott.assert_almost_equal(beta, 0.111942, 1e-5, 0.0)
    else:
        ott.assert_almost_equal(beta, 0.037943, 1e-5, 0.0)
    (
        interval,
        beta,
    ) = distribution.computeBilateralConfidenceIntervalWithMarginalProbability(0.95)
    if distribution.getK() == 1.5:
        ott.assert_almost_equal(interval.getLowerBound(), [-0.456841], 1e-5, 0.0)
        ott.assert_almost_equal(interval.getUpperBound(), [1.36968], 1e-5, 0.0)
    else:
        ott.assert_almost_equal(interval.getLowerBound(), [3.35815], 1e-5, 0.0)
        ott.assert_almost_equal(interval.getUpperBound(), [9.39585], 1e-5, 0.0)
    ott.assert_almost_equal(beta, 0.95, 1e-5, 0.0)
    (
        interval,
        beta,
    ) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(
        0.95, False
    )
    if distribution.getK() == 1.5:
        ott.assert_almost_equal(interval.getLowerBound(), [-0.5], 1e-5, 0.0)
        ott.assert_almost_equal(interval.getUpperBound(), [1.06295], 1e-5, 0.0)
    else:
        ott.assert_almost_equal(interval.getLowerBound(), [0.0], 0.0, 0.0)
        ott.assert_almost_equal(interval.getUpperBound(), [8.75459], 1e-5, 0.0)
    ott.assert_almost_equal(beta, 0.95, 1e-5, 0.0)
    (
        interval,
        beta,
    ) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(
        0.95, True
    )
    if distribution.getK() == 1.5:
        ott.assert_almost_equal(interval.getLowerBound(), [-0.429631], 1e-5, 0.0)
        ott.assert_almost_equal(interval.getUpperBound(), [13.1546], 1e-5, 0.0)
    else:
        ott.assert_almost_equal(interval.getLowerBound(), [3.69853], 1e-5, 0.0)
        ott.assert_almost_equal(interval.getUpperBound(), [26.3665], 1e-5, 0.0)
    ott.assert_almost_equal(beta, 0.95, 1e-5, 0.0)

    mean = distribution.getMean()
    if distribution.getK() == 1.5:
        ott.assert_almost_equal(mean, [0.1], 1e-5, 0.0)
    else:
        ott.assert_almost_equal(mean, [6.0], 1e-5, 0.0)
    covariance = distribution.getCovariance()
    if distribution.getK() == 1.5:
        ott.assert_almost_equal(covariance, ot.CovarianceMatrix(1, [0.24]), 1e-5, 0.0)
    else:
        ott.assert_almost_equal(covariance, ot.CovarianceMatrix(1, [2.4]), 1e-5, 0.0)
    correlation = distribution.getCorrelation()
    ott.assert_almost_equal(correlation, ot.CorrelationMatrix(1), 1e-5, 0.0)
    spearman = distribution.getSpearmanCorrelation()
    ott.assert_almost_equal(spearman, ot.CorrelationMatrix(1), 1e-5, 0.0)
    kendall = distribution.getKendallTau()
    ott.assert_almost_equal(kendall, ot.CorrelationMatrix(1), 1e-5, 0.0)
    parameters = distribution.getParametersCollection()
    assert len(parameters) == 1
    if distribution.getK() == 1.5:
        ott.assert_almost_equal(parameters[0], [1.5, 2.5, -0.5], 0.0, 0.0)
    else:
        ott.assert_almost_equal(parameters[0], [15.0, 2.5, 0.0], 0.0, 0.0)
    assert distribution.getStandardRepresentative().getImplementation().getClassName() == "Gamma"

    standardDeviation = distribution.getStandardDeviation()
    if distribution.getK() == 1.5:
        ott.assert_almost_equal(standardDeviation, [0.489898], 1e-5, 0.0)
    else:
        ott.assert_almost_equal(standardDeviation, [1.54919], 1e-5, 0.0)
    skewness = distribution.getSkewness()
    if distribution.getK() == 1.5:
        ott.assert_almost_equal(skewness, [1.63299], 1e-5, 0.0)
    else:
        ott.assert_almost_equal(skewness, [0.516398], 1e-5, 0.0)
    kurtosis = distribution.getKurtosis()
    if distribution.getK() == 1.5:
        ott.assert_almost_equal(kurtosis, [7.0], 1e-5, 0.0)
    else:
        ott.assert_almost_equal(kurtosis, [3.4], 1e-5, 0.0)

    ot.Log.Show(ot.Log.TRACE)
    validation = ott.DistributionValidation(distribution)
    validation.run()

# Density at the support lower bound for k = 1: lambda
edge = ot.Gamma(1.0, 2.0)
ott.assert_almost_equal(edge.computePDF(0.0), 2.0, 1e-12, 0.0)
ott.assert_almost_equal(edge.computeLogPDF(0.0), math.log(2.0), 1e-12, 0.0)
# Roughness: analytic value lambda * Gamma(2k-1) / (2^(2k-1) * Gamma(k)^2)
ott.assert_almost_equal(ot.Gamma(2.0, 3.0).getRoughness(), 0.75)
# The L2 norm of the PDF diverges for k <= 0.5
with ott.assert_raises(Exception):
    ot.Gamma(0.5, 1.0).getRoughness()

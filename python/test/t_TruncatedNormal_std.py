#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Instantiate one distribution object
for distribution in [
    ot.TruncatedNormal(1.5, 3.0, -2.0, 5.0),
    ot.TruncatedNormal(50.0, 1.0, 3.0, 4.0),
]:
    assert distribution.getClassName() == "TruncatedNormal"

    # Is this distribution elliptical ?
    if distribution.getMu()[0] == 1.5:
        assert distribution.isElliptical()
    else:
        assert not distribution.isElliptical()

    # Is this distribution continuous ?
    assert distribution.isContinuous()

    # Test for realization of distribution
    oneRealization = distribution.getRealization()
    if distribution.getMu()[0] == 1.5:
        ott.assert_almost_equal(oneRealization, [3.3246], 1e-5, 0.0)
    else:
        ott.assert_almost_equal(oneRealization, [3.96641], 1e-5, 0.0)

    # Define a point
    point = ot.Point(distribution.getDimension(), 3.5)
    ott.assert_almost_equal(point, [3.5], 0.0, 0.0)

    # derivative of PDF with regards its arguments
    DDF = distribution.computeDDF(point)
    if distribution.getMu()[0] == 1.5:
        ott.assert_almost_equal(DDF, [-0.0312729], 1e-5, 0.0)
    else:
        ott.assert_almost_equal(DDF, [1.93801e-07], 1e-5, 0.0)

    # PDF value
    LPDF = distribution.computeLogPDF(point)
    if distribution.getMu()[0] == 1.5:
        ott.assert_almost_equal(LPDF, -1.960925, 1e-6, 1e-6)
    else:
        ott.assert_almost_equal(LPDF, -19.295887, 1e-6, 1e-6)
    PDF = distribution.computePDF(point)
    if distribution.getMu()[0] == 1.5:
        ott.assert_almost_equal(PDF, 0.140728, 1e-6, 1e-6)
    else:
        ott.assert_almost_equal(PDF, 0.0, 0.0, 1e-6)

    # derivative of the PDF with regards the parameters of the distribution
    CDF = distribution.computeCDF(point)
    if distribution.getMu()[0] == 1.5:
        ott.assert_almost_equal(CDF, 0.827107, 1e-6, 1e-6)
    else:
        ott.assert_almost_equal(CDF, 0.0, 0.0, 1e-6)
    CCDF = distribution.computeComplementaryCDF(point)
    if distribution.getMu()[0] == 1.5:
        ott.assert_almost_equal(CCDF, 0.172893, 1e-6, 1e-6)
    else:
        ott.assert_almost_equal(CCDF, 1.0, 1e-6, 1e-6)

    PDFgr = distribution.computePDFGradient(point)
    if distribution.getMu()[0] == 1.5:
        ott.assert_almost_equal(PDFgr, [0.0312729, 0.0031597, 0.0125231, -0.0125231], 1e-5, 0.0)
    else:
        ott.assert_almost_equal(PDFgr, [-1.99336e-09, 1.84427e-07, 0.0, -1.91808e-07], 1e-5, 0.0)

    # derivative of the logPDF with regards the parameters of the distribution
    logPDFgr = distribution.computeLogPDFGradient(point)
    if distribution.getMu()[0] == 1.5:
        ott.assert_almost_equal(logPDFgr, [0.222222, 0.0224525, 0.0889876, -0.0889876], 1e-5, 0.0)
    else:
        ott.assert_almost_equal(logPDFgr, [-0.478281, 44.2509, 0.0, -46.0217], 1e-5, 0.0)

    # derivative of the PDF with regards the parameters of the distribution
    CDFgr = distribution.computeCDFGradient(point)
    if distribution.getMu()[0] == 1.5:
        ott.assert_almost_equal(CDFgr, [-0.0517406, -0.0258989, -0.0153853, -0.0736023], 1e-5, 0.0)
    else:
        ott.assert_almost_equal(CDFgr, [-4.47731e-11, 4.14344e-09, -2.95456e-19, -4.12299e-09], 1e-5, 0.0)

    # quantile
    quantile = distribution.computeQuantile(0.95)
    if distribution.getMu()[0] == 1.5:
        ott.assert_almost_equal(quantile, [4.48948], 1e-5, 0.0)
    else:
        ott.assert_almost_equal(quantile, [3.99889], 1e-5, 0.0)
    ott.assert_almost_equal(distribution.computeCDF(quantile), 0.95, 1e-6, 1e-6)
    # Get 95% survival function
    inverseSurvival = ot.Point(distribution.computeInverseSurvivalFunction(0.95))
    if distribution.getMu()[0] == 1.5:
        ott.assert_almost_equal(inverseSurvival, [-1.48948], 1e-5, 0.0)
    else:
        ott.assert_almost_equal(inverseSurvival, [3.93495], 1e-5, 0.0)
    ott.assert_almost_equal(
        distribution.computeSurvivalFunction(inverseSurvival), 0.95, 1e-6, 1e-6
    )
    if distribution.getMu()[0] == 1.5:
        ott.assert_almost_equal(distribution.computeEntropy(), 1.927246, 1e-6, 1e-6)
    else:
        ott.assert_almost_equal(distribution.computeEntropy(), -2.829585, 1e-6, 1e-6)

    # Confidence regions
    interval, threshold = distribution.computeMinimumVolumeIntervalWithMarginalProbability(
        0.95
    )
    if distribution.getMu()[0] == 1.5:
        ott.assert_almost_equal(interval.getLowerBound(), [-1.73303], 1e-5, 0.0)
        ott.assert_almost_equal(interval.getUpperBound(), [4.73303], 1e-5, 0.0)
    else:
        ott.assert_almost_equal(interval.getLowerBound(), [3.93495], 1e-5, 0.0)
        ott.assert_almost_equal(interval.getUpperBound(), [4.0], 1e-5, 0.0)
    ott.assert_almost_equal(threshold, 0.95, 1e-5, 0.0)
    levelSet, beta = distribution.computeMinimumVolumeLevelSetWithThreshold(0.95)
    if distribution.getMu()[0] == 1.5:
        ott.assert_almost_equal(beta, [0.0983328], 1e-5, 0.0)
    else:
        ott.assert_almost_equal(beta, [2.30434], 1e-5, 0.0)
    interval, beta = distribution.computeBilateralConfidenceIntervalWithMarginalProbability(
        0.95
    )
    if distribution.getMu()[0] == 1.5:
        ott.assert_almost_equal(interval.getLowerBound(), [-1.73303], 1e-5, 0.0)
        ott.assert_almost_equal(interval.getUpperBound(), [4.73303], 1e-5, 0.0)
    else:
        ott.assert_almost_equal(interval.getLowerBound(), [3.91991], 1e-5, 0.0)
        ott.assert_almost_equal(interval.getUpperBound(), [3.99945], 1e-5, 0.0)
    ott.assert_almost_equal(beta, 0.95, 1e-5, 0.0)
    interval, beta = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(
        0.95, False
    )
    if distribution.getMu()[0] == 1.5:
        ott.assert_almost_equal(interval.getLowerBound(), [-2.0], 1e-5, 0.0)
        ott.assert_almost_equal(interval.getUpperBound(), [4.48948], 1e-5, 0.0)
    else:
        ott.assert_almost_equal(interval.getLowerBound(), [3.0], 1e-5, 0.0)
        ott.assert_almost_equal(interval.getUpperBound(), [3.99889], 1e-5, 0.0)
    ott.assert_almost_equal(beta, 0.95, 1e-5, 0.0)
    interval, beta = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(
        0.95, True
    )
    if distribution.getMu()[0] == 1.5:
        ott.assert_almost_equal(interval.getLowerBound(), [-1.48948], 1e-5, 0.0)
        ott.assert_almost_equal(interval.getUpperBound(), [5.0], 1e-5, 0.0)
    else:
        ott.assert_almost_equal(interval.getLowerBound(), [3.93495], 1e-5, 0.0)
        ott.assert_almost_equal(interval.getUpperBound(), [4.0], 1e-5, 0.0)
    ott.assert_almost_equal(beta, 0.95, 1e-5, 0.0)

    mean = distribution.getMean()
    if distribution.getMu()[0] == 1.5:
        ott.assert_almost_equal(mean, [1.5], 1e-5, 0.0)
    else:
        ott.assert_almost_equal(mean, [3.97828], 1e-5, 0.0)
    standardDeviation = distribution.getStandardDeviation()
    if distribution.getMu()[0] == 1.5:
        ott.assert_almost_equal(standardDeviation, [1.84222], 1e-5, 0.0)
    else:
        ott.assert_almost_equal(standardDeviation, [0.0217084], 1e-5, 0.0)
    covariance = distribution.getCovariance()
    if distribution.getMu()[0] == 1.5:
        ott.assert_almost_equal(covariance, ot.CovarianceMatrix(1, [3.39378]), 1e-5, 0.0)
    else:
        ott.assert_almost_equal(covariance, ot.CovarianceMatrix(1, [0.000471255]), 1e-5, 0.0)
    parameters = distribution.getParametersCollection()
    assert len(parameters) == 1
    if distribution.getMu()[0] == 1.5:
        ott.assert_almost_equal(parameters[0], [1.5, 3.0, -2.0, 5.0], 0.0, 0.0)
    else:
        ott.assert_almost_equal(parameters[0], [50.0, 1.0, 3.0, 4.0], 0.0, 0.0)
    assert distribution.getStandardRepresentative().getImplementation().getClassName() == "TruncatedNormal"

    ot.Log.Show(ot.Log.TRACE)
    validation = ott.DistributionValidation(distribution)
    validation.skipPDFAtLowerBound()  # the density is the renormalized normal density at the finite lower bound
    validation.skipCharacteristicFunction()
    validation.skipEntropy()  # slow
    validation.skipMinimumVolumeLevelSet()  # slow
    if (
        ot.Normal(distribution.getMu(), distribution.getSigma()).computeProbability(
            distribution.getRange()
        )
        < 0.25
    ):
        # when rejection becomes too costly
        validation.skipMoments()
        validation.skipCorrelation()
        validation.skipConfidenceInterval()
    validation.run()

# Issue #3024: far right-tail truncation with normal scale
t = ot.TruncatedNormal(0.0, 1.0, 10.0, 39.0)
ott.assert_almost_equal(t.computePDF(10.1), 3.6963528500427416, 1e-12)
ott.assert_almost_equal(t.computeCDF(10.1), 0.6375114502856419, 1e-12)
ott.assert_almost_equal(t.computeComplementaryCDF(10.1), 0.36248854971435807, 1e-12)
# Symmetrical left-tail case
t2 = ot.TruncatedNormal(0.0, 1.0, -39.0, -10.0)
ott.assert_almost_equal(t2.computePDF(-10.1), 3.6963528500427416, 1e-12)
ott.assert_almost_equal(t2.computeCDF(-10.1), 0.36248854971435807, 1e-12)
# quantile/cdf failure with low-probability truncation interval
t3 = ot.TruncatedNormal(-52.0658, 1.56137, -2.22475e-05, 0.1408)
ott.assert_almost_equal(t3.computeScalarQuantile(0.5), 0.030153711943197572, 1e-12)
ott.assert_almost_equal(t3.computeCDF(0.0), 0.00049999351035801, 1e-12)

# Half-normal: TruncatedNormal(0, 1, 0, +inf)
t4 = ot.TruncatedNormal(0.0, 1.0, 0.0, ot.SpecFunc.Infinity)
ott.assert_almost_equal(t4.getMean()[0], 0.7978845608028654, 1e-12)
ott.assert_almost_equal(t4.getStandardDeviation()[0], 0.6028102749890869, 1e-12)
ott.assert_almost_equal(t4.getCovariance()[0, 0], 0.36338022763241865, 1e-12)
# Symmetric left-tail case: skewness should be negated
t5 = ot.TruncatedNormal(0.0, 1.0, -ot.SpecFunc.Infinity, 0.0)
ott.assert_almost_equal(t5.getStandardDeviation()[0], 0.6028102749890869, 1e-12)
ott.assert_almost_equal(t5.getSkewness()[0], -t4.getSkewness()[0], 1e-12)
# Both bounds infinite: should be standard normal
t6 = ot.TruncatedNormal(0.0, 1.0, -ot.SpecFunc.Infinity, ot.SpecFunc.Infinity)
ott.assert_almost_equal(t6.getStandardDeviation()[0], 1.0, 1e-12)
ott.assert_almost_equal(t6.getSkewness()[0], 0.0, 1e-12)
ott.assert_almost_equal(t6.getKurtosis()[0], 3.0, 1e-12)

# Density at the finite bounds: renormalized normal density
edge = ot.TruncatedNormal(1.5, 3.0, -2.0, 5.0)
ref = ot.Normal(1.5, 3.0)
ott.assert_almost_equal(
    edge.computePDF(-2.0), ref.computePDF(-2.0) / (ref.computeCDF(5.0) - ref.computeCDF(-2.0)), 1e-12, 0.0
)
ott.assert_almost_equal(edge.computePDF(5.0), edge.computePDF(-2.0), 1e-12, 0.0)
assert edge.computePDF(-2.0) > 0.0
# Right-sided DDF and inside PDF gradient at the finite lower bound
ott.assert_almost_equal(edge.computeDDF(-2.0), [-(-2.0 - 1.5) / 9.0 * edge.computePDF(-2.0)], 1e-12, 0.0)
grad = edge.computePDFGradient(-2.0)
assert grad.norm() > 0.0
ott.assert_almost_equal(grad[2], -edge.computePDFGradient(5.0)[3], 1e-12, 0.0)

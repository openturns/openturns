#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import math as m

ot.TESTPREAMBLE()

# Instantiate one distribution object
for dim in range(1, 2):
    theta = ot.Point(dim + 1)
    for i in range(dim + 1):
        theta[i] = (i + 1.0) / 4.0
    distribution = ot.Dirichlet(theta)
    description = [""] * dim
    for j in range(1, dim + 1):
        oss = "Marginal " + str(j)
        description[j - 1] = oss
    distribution.setDescription(description)
    assert distribution.getClassName() == "Dirichlet"

    # Is this distribution elliptical ?
    assert not distribution.isElliptical()

    # Is this distribution continuous ?
    assert distribution.isContinuous()

    # Test for realization of distribution
    oneRealization = distribution.getRealization()
    ott.assert_almost_equal(oneRealization, [0.168034], 1e-6, 0.0)

    # Define a point
    point = ot.Point(distribution.getDimension(), 0.5 / distribution.getDimension())
    ott.assert_almost_equal(point, [0.5], 0.0, 0.0)

    # Show PDF and CDF of point
    LPDF = distribution.computeLogPDF(point)
    ott.assert_almost_equal(LPDF, -0.79067254, 1e-7, 0.0)
    PDF = distribution.computePDF(point)
    ott.assert_almost_equal(PDF, 0.45353967, 1e-7, 0.0)
    CDF = distribution.computeCDF(point)
    ott.assert_almost_equal(CDF, 0.6831129, 1e-7, 0.0)
    quantile = distribution.computeQuantile(0.95)
    ott.assert_almost_equal(quantile, [0.982959], 1e-6, 0.0)
    ott.assert_almost_equal(distribution.computeCDF(quantile), 0.95, 1e-6, 0.0)
    # Get 95% survival function
    inverseSurvival = ot.Point(distribution.computeInverseSurvivalFunction(0.95))
    ott.assert_almost_equal(inverseSurvival, [1.8464e-05], 1e-6, 1e-12)
    ott.assert_almost_equal(
        distribution.computeSurvivalFunction(inverseSurvival), 0.95, 1e-6, 0.0
    )
    ott.assert_almost_equal(distribution.computeEntropy(), -1.137913, 1e-6, 0.0)

    # Confidence regions
    (
        interval,
        threshold,
    ) = distribution.computeMinimumVolumeIntervalWithMarginalProbability(0.95)
    ott.assert_almost_equal(interval.getLowerBound(), [0.0], 0.0, 0.0)
    ott.assert_almost_equal(interval.getUpperBound(), [0.982959], 1e-6, 0.0)
    ott.assert_almost_equal(threshold, 0.95, 1e-12, 0.0)
    levelSet, beta = distribution.computeMinimumVolumeLevelSetWithThreshold(0.95)
    ott.assert_almost_equal(beta, 0.445853, 1e-6, 0.0)
    (
        interval,
        beta,
    ) = distribution.computeBilateralConfidenceIntervalWithMarginalProbability(0.95)
    ott.assert_almost_equal(interval.getLowerBound(), [1.1540077688761149e-06], 1e-12, 0.0)
    ott.assert_almost_equal(interval.getUpperBound(), [0.995712], 1e-6, 0.0)
    ott.assert_almost_equal(beta, 0.95, 1e-12, 0.0)
    (
        interval,
        beta,
    ) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(
        0.95, False
    )
    ott.assert_almost_equal(interval.getLowerBound(), [0.0], 0.0, 0.0)
    ott.assert_almost_equal(interval.getUpperBound(), [0.982959], 1e-6, 0.0)
    ott.assert_almost_equal(beta, 0.95, 1e-12, 0.0)
    (
        interval,
        beta,
    ) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(
        0.95, True
    )
    ott.assert_almost_equal(interval.getLowerBound(), [1.8464e-05], 1e-6, 1e-12)
    ott.assert_almost_equal(interval.getUpperBound(), [1.0], 0.0, 0.0)
    ott.assert_almost_equal(beta, 0.95, 1e-12, 0.0)

    mean = distribution.getMean()
    ott.assert_almost_equal(mean, [1.0 / 3.0], 1e-12, 0.0)
    standardDeviation = distribution.getStandardDeviation()
    ott.assert_almost_equal(standardDeviation, [0.356348], 1e-6, 0.0)
    skewness = distribution.getSkewness()
    ott.assert_almost_equal(skewness, [0.680301], 1e-6, 0.0)
    kurtosis = distribution.getKurtosis()
    ott.assert_almost_equal(kurtosis, [1.90909], 1e-5, 0.0)
    covariance = distribution.getCovariance()
    ott.assert_almost_equal(
        covariance, ot.CovarianceMatrix(1, [0.12698412698412698]), 1e-12, 0.0
    )

    x = 0.6
    y = [0.2] * (dim - 1)
    ott.assert_almost_equal(distribution.computeConditionalPDF(x, y), 0.442267, 1e-6, 0.0)
    ott.assert_almost_equal(distribution.computeConditionalCDF(x, y), 0.727715, 1e-6, 0.0)
    ott.assert_almost_equal(
        distribution.computeConditionalQuantile(x, y), 0.329133, 1e-6, 0.0
    )
    pt = ot.Point([0.1 * i + 0.15 for i in range(dim)])
    ott.assert_almost_equal(
        distribution.computeSequentialConditionalPDF(point), [0.45354], 1e-5, 0.0
    )
    resCDF = distribution.computeSequentialConditionalCDF(pt)
    ott.assert_almost_equal(resCDF, [0.482299], 1e-6, 0.0)
    ott.assert_almost_equal(
        distribution.computeSequentialConditionalQuantile(resCDF), [0.15], 1e-12, 0.0
    )

    # Ticket #2306
    if dim == 2:
        condPDF = distribution.computeConditionalPDF(-0.1, [0.5])
        assert m.isfinite(condPDF), "condPDF is not normal"
        condPDF = distribution.computeConditionalPDF(0.5, [0.5])
        assert m.isfinite(condPDF), "condPDF is not normal"
        condPDF = distribution.computeConditionalPDF(0.6, [0.5])
        assert m.isfinite(condPDF), "condPDF is not normal"
        condPDF = distribution.computeSequentialConditionalPDF([0.5, -0.1])
        assert m.isfinite(condPDF), "condPDF is not normal"
        condPDF = distribution.computeSequentialConditionalPDF([0.5, 0.5])
        assert m.isfinite(condPDF), "condPDF is not normal"
        condPDF = distribution.computeSequentialConditionalPDF([0.5, 0.6])
        assert m.isfinite(condPDF), "condPDF is not normal"

    # Extract the marginals
    for i in range(dim):
        margin = distribution.getMarginal(i)
        ott.assert_almost_equal(margin.computePDF(ot.Point(1, 0.5)), 0.45353967, 1e-7, 0.0)
        ott.assert_almost_equal(margin.computeCDF(ot.Point(1, 0.5)), 0.6831129, 1e-7, 0.0)
        ott.assert_almost_equal(margin.computeQuantile(0.95), [0.982959], 1e-6, 0.0)
        ott.assert_almost_equal(margin.getRealization(), [0.2406935108379952], 1e-12, 0.0)
    if dim >= 2:
        # Extract a 2-D marginal
        indices = [1, 0]
        margins = distribution.getMarginal(indices)
        ott.assert_almost_equal(margins.computePDF(ot.Point(2, 0.5)), 0.45353967, 1e-7, 0.0)
        ott.assert_almost_equal(margins.computeCDF(ot.Point(2, 0.5)), 0.6831129, 1e-7, 0.0)
        quantile = margins.computeQuantile(0.95)
        ott.assert_almost_equal(quantile, [0.982959], 1e-6, 0.0)
        ott.assert_almost_equal(margins.computeCDF(quantile), 0.95, 1e-6, 0.0)

    ot.Log.Show(ot.Log.TRACE)
    validation = ott.DistributionValidation(distribution)
    validation.skipCDF()
    validation.skipGradient()
    validation.skipMoments()
    validation.run()

# check Dirichlet computeCDF has no global sample cache
theta1 = [2.0, 6.0, 3.0, 4.0, 5.0]
d1 = ot.Dirichlet(theta1)
theta2 = [10.0, 30.0, 15.0, 20.0, 25.0]
d2 = ot.Dirichlet(theta2)
point = [0.15] * 4
cdf1 = d1.computeCDF(point)
cdf2 = d2.computeCDF(point)
assert abs(cdf1 - cdf2) > 1e-8, "Dirichlet CDF should differ for different theta"

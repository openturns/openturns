#! /usr/bin/env python

import cmath
import math

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

for xi in [-0.2, 0.0, 0.2]:
    # Instantiate one distribution object
    distribution = ot.GeneralizedPareto(1.5, xi, 0.5)

    assert distribution.getClassName() == "GeneralizedPareto"

    if xi > 0.0:
        asPareto = distribution.asPareto()
        assert asPareto.getClassName() == "Pareto"
        ott.assert_almost_equal(asPareto.getParameter(), [7.5, 5.0, -7.0], 1e-12, 0.0)

    # Is this distribution elliptical ?
    assert not distribution.isElliptical()

    # Is this distribution continuous ?
    assert distribution.isContinuous()

    # Test for realization of distribution
    oneRealization = distribution.getRealization()
    if xi == -0.2:
        ott.assert_almost_equal(oneRealization, [1.16226], 1e-5, 0.0)
    elif xi == 0.0:
        ott.assert_almost_equal(oneRealization, [0.633808], 1e-5, 0.0)
    else:
        ott.assert_almost_equal(oneRealization, [0.635009], 1e-5, 0.0)

    # Test for sampling
    size = 10000
    oneSample = distribution.getSample(size)
    if xi == -0.2:
        ott.assert_almost_equal(oneSample[0], [0.684665], 1e-5, 0.0)
        ott.assert_almost_equal(oneSample[size - 1], [1.68051], 1e-5, 0.0)
        ott.assert_almost_equal(oneSample.computeMean(), [1.74428], 1e-5, 0.0)
        ott.assert_almost_equal(
            oneSample.computeCovariance(), ot.CovarianceMatrix(1, [1.12783]), 1e-5, 0.0
        )
    elif xi == 0.0:
        ott.assert_almost_equal(oneSample[0], [5.22134], 1e-5, 0.0)
        ott.assert_almost_equal(oneSample[size - 1], [1.0016], 1e-5, 0.0)
        ott.assert_almost_equal(oneSample.computeMean(), [1.97709], 1e-5, 0.0)
        ott.assert_almost_equal(
            oneSample.computeCovariance(), ot.CovarianceMatrix(1, [2.14949]), 1e-5, 0.0
        )
    else:
        ott.assert_almost_equal(oneSample[0], [7.07521], 1e-5, 0.0)
        ott.assert_almost_equal(oneSample[size - 1], [1.01876], 1e-5, 0.0)
        ott.assert_almost_equal(oneSample.computeMean(), [2.33451], 1e-5, 0.0)
        ott.assert_almost_equal(
            oneSample.computeCovariance(), ot.CovarianceMatrix(1, [5.26239]), 1e-5, 0.0
        )

    # Define a point
    point = ot.Point(distribution.getDimension(), 1.5)
    ott.assert_almost_equal(point, [1.5], 0.0, 0.0)

    # DDF = distribution.computeDDF( point )

    PDF = distribution.computePDF(point)
    if xi == -0.2:
        ott.assert_almost_equal(PDF, 0.376111934156, 1e-9, 0.0)
    elif xi == 0.0:
        ott.assert_almost_equal(PDF, 0.342278079355, 1e-9, 0.0)
    else:
        ott.assert_almost_equal(PDF, 0.31460293288, 1e-9, 0.0)

    CDF = distribution.computeCDF(point)
    if xi == -0.2:
        ott.assert_almost_equal(CDF, 0.511054485597, 1e-9, 0.0)
    elif xi == 0.0:
        ott.assert_almost_equal(CDF, 0.486582880967, 1e-9, 0.0)
    else:
        ott.assert_almost_equal(CDF, 0.465175014104, 1e-9, 0.0)
    PDFgr = distribution.computePDFGradient(point)
    if xi == -0.2:
        ott.assert_almost_equal(PDFgr, [-0.096439, -0.188281, 0.231453], 1e-5, 0.0)
    elif xi == 0.0:
        ott.assert_almost_equal(PDFgr, [-0.0760618, -0.152124, 0.228185], 1e-5, 0.0)
    else:
        ott.assert_almost_equal(PDFgr, [-0.0616868, -0.125946, 0.222073], 1e-5, 0.0)

    CDFgr = distribution.computeCDFGradient(point)
    if xi == -0.2:
        ott.assert_almost_equal(CDFgr, [-0.250741, -0.131347, -0.376112], 1e-5, 0.0)
    elif xi == 0.0:
        ott.assert_almost_equal(CDFgr, [-0.228185, -0.114093, -0.342278], 1e-5, 0.0)
    else:
        ott.assert_almost_equal(CDFgr, [-0.209735, -0.100495, -0.314603], 1e-5, 0.0)

    quantile = distribution.computeQuantile(0.95)
    if xi == -0.2:
        ott.assert_almost_equal(quantile, [3.8804], 1e-5, 0.0)
    elif xi == 0.0:
        ott.assert_almost_equal(quantile, [4.9936], 1e-5, 0.0)
    else:
        ott.assert_almost_equal(quantile, [6.65423], 1e-5, 0.0)
    ott.assert_almost_equal(distribution.computeCDF(quantile), 0.95, 1e-9, 0.0)
    # Get 95% survival function
    inverseSurvival = ot.Point(distribution.computeInverseSurvivalFunction(0.95))
    if xi == -0.2:
        ott.assert_almost_equal(inverseSurvival, [0.576547], 1e-5, 0.0)
    elif xi == 0.0:
        ott.assert_almost_equal(inverseSurvival, [0.57694], 1e-5, 0.0)
    else:
        ott.assert_almost_equal(inverseSurvival, [0.577336], 1e-5, 0.0)
    ott.assert_almost_equal(
        distribution.computeSurvivalFunction(inverseSurvival), 0.95, 1e-6, 1e-6
    )
    if xi == -0.2:
        ott.assert_almost_equal(distribution.computeEntropy(), 1.205465, 1e-6, 1e-6)
    elif xi == 0.0:
        ott.assert_almost_equal(distribution.computeEntropy(), 1.405465, 1e-6, 1e-6)
    else:
        ott.assert_almost_equal(distribution.computeEntropy(), 1.605465, 1e-6, 1e-6)

    # Confidence regions
    (
        interval,
        threshold,
    ) = distribution.computeMinimumVolumeIntervalWithMarginalProbability(0.95)
    if xi == -0.2:
        ott.assert_almost_equal(interval.getLowerBound(), [0.5], 1e-5, 0.0)
        ott.assert_almost_equal(interval.getUpperBound(), [3.8804], 1e-5, 0.0)
    elif xi == 0.0:
        ott.assert_almost_equal(interval.getLowerBound(), [0.5], 1e-5, 0.0)
        ott.assert_almost_equal(interval.getUpperBound(), [4.9936], 1e-5, 0.0)
    else:
        ott.assert_almost_equal(interval.getLowerBound(), [0.5], 1e-5, 0.0)
        ott.assert_almost_equal(interval.getUpperBound(), [6.65423], 1e-5, 0.0)
    ott.assert_almost_equal(threshold, 0.95, 1e-5, 0.0)
    levelSet, beta = distribution.computeMinimumVolumeLevelSetWithThreshold(0.95)
    if xi == -0.2:
        ott.assert_almost_equal(beta, 0.0606855, 1e-5, 0.0)
    elif xi == 0.0:
        ott.assert_almost_equal(beta, 0.0333333, 1e-5, 0.0)
    else:
        ott.assert_almost_equal(beta, 0.0183093, 1e-5, 0.0)
    (
        interval,
        beta,
    ) = distribution.computeBilateralConfidenceIntervalWithMarginalProbability(0.95)
    if xi == -0.2:
        ott.assert_almost_equal(interval.getLowerBound(), [0.537881], 1e-5, 0.0)
        ott.assert_almost_equal(interval.getUpperBound(), [4.41368], 1e-5, 0.0)
    elif xi == 0.0:
        ott.assert_almost_equal(interval.getLowerBound(), [0.537977], 1e-5, 0.0)
        ott.assert_almost_equal(interval.getUpperBound(), [6.03332], 1e-5, 0.0)
    else:
        ott.assert_almost_equal(interval.getLowerBound(), [0.538073], 1e-5, 0.0)
        ott.assert_almost_equal(interval.getUpperBound(), [8.68459], 1e-5, 0.0)
    ott.assert_almost_equal(beta, 0.95, 1e-5, 0.0)
    (
        interval,
        beta,
    ) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(
        0.95, False
    )
    if xi == -0.2:
        ott.assert_almost_equal(interval.getLowerBound(), [0.5], 1e-5, 0.0)
        ott.assert_almost_equal(interval.getUpperBound(), [3.8804], 1e-5, 0.0)
    elif xi == 0.0:
        ott.assert_almost_equal(interval.getLowerBound(), [0.5], 1e-5, 0.0)
        ott.assert_almost_equal(interval.getUpperBound(), [4.9936], 1e-5, 0.0)
    else:
        ott.assert_almost_equal(interval.getLowerBound(), [0.5], 1e-5, 0.0)
        ott.assert_almost_equal(interval.getUpperBound(), [6.65423], 1e-5, 0.0)
    ott.assert_almost_equal(beta, 0.95, 1e-5, 0.0)
    (
        interval,
        beta,
    ) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(
        0.95, True
    )
    if xi == -0.2:
        ott.assert_almost_equal(interval.getLowerBound(), [0.576547], 1e-5, 0.0)
        ott.assert_almost_equal(interval.getUpperBound(), [8.0], 1e-5, 0.0)
    elif xi == 0.0:
        ott.assert_almost_equal(interval.getLowerBound(), [0.57694], 1e-5, 0.0)
        ott.assert_almost_equal(interval.getUpperBound(), [54.5655], 1e-5, 0.0)
    else:
        ott.assert_almost_equal(interval.getLowerBound(), [0.577336], 1e-5, 0.0)
        ott.assert_almost_equal(interval.getUpperBound(), [10126.8], 1e-5, 0.0)
    ott.assert_almost_equal(beta, 0.95, 1e-5, 0.0)

    mean = distribution.getMean()
    if xi == -0.2:
        ott.assert_almost_equal(mean, [1.75], 1e-5, 0.0)
    elif xi == 0.0:
        ott.assert_almost_equal(mean, [2.0], 1e-5, 0.0)
    else:
        ott.assert_almost_equal(mean, [2.375], 1e-5, 0.0)
    standardDeviation = distribution.getStandardDeviation()
    if xi == -0.2:
        ott.assert_almost_equal(standardDeviation, [1.05644], 1e-5, 0.0)
    elif xi == 0.0:
        ott.assert_almost_equal(standardDeviation, [1.5], 1e-5, 0.0)
    else:
        ott.assert_almost_equal(standardDeviation, [2.42061], 1e-5, 0.0)
    skewness = distribution.getSkewness()
    if xi == -0.2:
        ott.assert_almost_equal(skewness, [1.18322], 1e-5, 0.0)
    elif xi == 0.0:
        ott.assert_almost_equal(skewness, [2.0], 1e-5, 0.0)
    else:
        ott.assert_almost_equal(skewness, [4.64758], 1e-5, 0.0)
    kurtosis = distribution.getKurtosis()
    if xi == -0.2:
        ott.assert_almost_equal(kurtosis, [4.2], 1e-5, 0.0)
    elif xi == 0.0:
        ott.assert_almost_equal(kurtosis, [9.0], 1e-5, 0.0)
    else:
        ott.assert_almost_equal(kurtosis, [73.8], 1e-5, 0.0)
    covariance = distribution.getCovariance()
    if xi == -0.2:
        ott.assert_almost_equal(covariance, ot.CovarianceMatrix(1, [1.11607]), 1e-5, 0.0)
    elif xi == 0.0:
        ott.assert_almost_equal(covariance, ot.CovarianceMatrix(1, [2.25]), 1e-5, 0.0)
    else:
        ott.assert_almost_equal(covariance, ot.CovarianceMatrix(1, [5.85938]), 1e-5, 0.0)
    parameters = distribution.getParametersCollection()
    assert len(parameters) == 1
    if xi == -0.2:
        ott.assert_almost_equal(parameters[0], [1.5, -0.2, 0.5], 0.0, 0.0)
    elif xi == 0.0:
        ott.assert_almost_equal(parameters[0], [1.5, 0.0, 0.5], 0.0, 0.0)
    else:
        ott.assert_almost_equal(parameters[0], [1.5, 0.2, 0.5], 0.0, 0.0)
    assert distribution.getStandardRepresentative().getImplementation().getClassName() == "GeneralizedPareto"

    ot.Log.Show(ot.Log.TRACE)
    ot.RandomGenerator.SetSeed(1)
    validation = ott.DistributionValidation(distribution)
    validation.skipPDFAtLowerBound()  # the density is 1 / sigma at the finite lower bound
    validation.run()

# Density at the finite lower bound u: 1 / sigma
ott.assert_almost_equal(distribution.computePDF(0.5), 1.0 / 1.5, 1e-12, 0.0)
ott.assert_almost_equal(distribution.computeLogPDF(0.5), -math.log(1.5), 1e-12, 0.0)

# Test computeCharacteristicFunction with xi=0 (regression test for missing sigma/u)
gpd = ot.GeneralizedPareto(1.5, 0.0, 0.5)
x = 1.2
cf = gpd.computeCharacteristicFunction(x)
# Expected: exp(i*u*x) / (1 - i*sigma*x) = exp(0.6i) / (1 - 1.8i)
expected = cmath.exp(complex(0.0, 0.5 * x)) / complex(1.0, -1.5 * x)
ott.assert_almost_equal(cf, expected, 1e-14, 0.0)

#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Instantiate one distribution object
distribution = ot.UniformOrderStatistics(4)
assert distribution.getClassName() == "UniformOrderStatistics"

# Is this distribution elliptical ?
assert not distribution.isElliptical()

# Is this distribution continuous ?
assert distribution.isContinuous()

# Test for realization of distribution
oneRealization = distribution.getRealization()
ott.assert_almost_equal(oneRealization, [0.128996, 0.204795, 0.217965, 0.4246], 1e-5, 0.0)

# Define a point
point = [0.1, 0.15, 0.25, 0.45]

# Show PDF and CDF of point
DDF = distribution.computeDDF(point)
ott.assert_almost_equal(DDF, [0.0, 0.0, 0.0, 0.0], 0.0, 0.0)
LPDF = distribution.computeLogPDF(point)
ott.assert_almost_equal(LPDF, 3.17805, 1e-5, 0.0)
PDF = distribution.computePDF(point)
ott.assert_almost_equal(PDF, 24.0, 1e-12, 0.0)
CDF = distribution.computeCDF(point)
ott.assert_almost_equal(CDF, 1.04e-02, 1e-5, 0.0)
# Too expensive for a test
CCDF = distribution.computeComplementaryCDF(point)
ott.assert_almost_equal(CCDF, 9.896e-01, 1e-5, 0.0)
Survival = distribution.computeSurvivalFunction(point)
ott.assert_almost_equal(Survival, 6.25831e-01, 1e-5, 0.0)
InverseSurvival = distribution.computeInverseSurvivalFunction(0.95)
ott.assert_almost_equal(
    InverseSurvival, [0.00386121, 0.0524168, 0.163562, 0.35202], 1e-5, 0.0
)
ott.assert_almost_equal(
    distribution.computeSurvivalFunction(InverseSurvival), 0.95, 1e-5, 0.0
)
quantile = distribution.computeQuantile(0.95)
ott.assert_almost_equal(quantile, [0.64798, 0.836438, 0.947583, 0.996139], 1e-5, 0.0)
ott.assert_almost_equal(distribution.computeCDF(quantile), 0.95, 1e-5, 0.0)

mean = distribution.getMean()
ott.assert_almost_equal(mean, [0.2, 0.4, 0.6, 0.8], 1e-12, 0.0)
standardDeviation = distribution.getStandardDeviation()
ott.assert_almost_equal(standardDeviation, [0.163299, 0.2, 0.2, 0.163299], 1e-5, 0.0)
skewness = distribution.getSkewness()
ott.assert_almost_equal(skewness, [1.04978, 0.285714, -0.285714, -1.04978], 1e-5, 0.0)
kurtosis = distribution.getKurtosis()
ott.assert_almost_equal(kurtosis, [3.69643, 2.35714, 2.35714, 3.69643], 1e-5, 0.0)
covariance = distribution.getCovariance()
ott.assert_almost_equal(
    covariance,
    ot.CovarianceMatrix(
        4,
        [
            0.0266667,
            0.02,
            0.0133333,
            0.00666667,
            0.02,
            0.04,
            0.0266667,
            0.0133333,
            0.0133333,
            0.0266667,
            0.04,
            0.02,
            0.00666667,
            0.0133333,
            0.02,
            0.0266667,
        ],
    ),
    1e-5,
    0.0,
)
correlation = distribution.getCorrelation()
ott.assert_almost_equal(
    correlation,
    ot.CorrelationMatrix(
        4,
        [
            1,
            0.612372,
            0.408248,
            0.25,
            0.612372,
            1,
            0.666667,
            0.408248,
            0.408248,
            0.666667,
            1,
            0.612372,
            0.25,
            0.408248,
            0.612372,
            1,
        ],
    ),
    1e-5,
    0.0,
)
# Too slow for a test
# spearman = distribution.getSpearmanCorrelation()
# kendall = distribution.getKendallTau()
parameters = distribution.getParametersCollection()
assert len(parameters) == 1
standard = distribution.getStandardRepresentative()
assert standard.getImplementation().getClassName() == "UniformOrderStatistics"

ot.Log.Show(ot.Log.TRACE)
validation = ott.DistributionValidation(distribution)
validation.skipCorrelation()
validation.skipMoments()
validation.run()

#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Instantiate one distribution object
first = -1.5
ll = [1.0, 0.7, 1.2, 0.9]
h = [0.5, 1.5, 3.5, 2.5]
t = [first]
f = list()
for i in range(len(ll)):
    t.append(t[i] + ll[i])
    f.append(h[i] * ll[i])
distribution = ot.Histogram(t, f)
assert distribution.getClassName() == "Histogram"
distribution = ot.Histogram(first, ll, h)
assert distribution.getClassName() == "Histogram"

# Is this distribution elliptical ?
assert not distribution.isElliptical()

# Is this distribution continuous ?
assert distribution.isContinuous()

# Test for realization of distribution
oneRealization = distribution.getRealization()
ott.assert_almost_equal(oneRealization, [1.19686], 1e-5, 0.0)

# Define a point
point = ot.Point(distribution.getDimension(), 1.0)
ott.assert_almost_equal(point, [1.0], 0.0, 0.0)

# derivative of PDF with regards its arguments
DDF = distribution.computeDDF(point)
ott.assert_almost_equal(DDF, [0.0], 1e-12, 0.0)

# PDF value
LPDF = distribution.computeLogPDF(point)
ott.assert_almost_equal(LPDF, -0.826679, 1e-6, 1e-6)
PDF = distribution.computePDF(point)
ott.assert_almost_equal(PDF, 0.437500, 1e-6, 1e-6)

# derivative of the PDF with regards the parameters of the distribution
CDF = distribution.computeCDF(point)
ott.assert_almost_equal(CDF, 0.543750, 1e-6, 1e-6)
CCDF = distribution.computeComplementaryCDF(point)
ott.assert_almost_equal(CCDF, 0.456250, 1e-6, 1e-6)

# quantile
quantile = distribution.computeQuantile(0.95)
ott.assert_almost_equal(quantile, [2.14], 1e-5, 0.0)
ott.assert_almost_equal(distribution.computeCDF(quantile), 0.95, 1e-6, 1e-6)
# Get 95% survival function
inverseSurvival = ot.Point(distribution.computeInverseSurvivalFunction(0.95))
ott.assert_almost_equal(inverseSurvival, [-0.7], 1e-5, 0.0)
ott.assert_almost_equal(
    distribution.computeSurvivalFunction(inverseSurvival), 0.95, 1e-6, 1e-6
)
ott.assert_almost_equal(distribution.computeEntropy(), 1.154139, 1e-6, 1e-6)

# Confidence regions
interval, threshold = distribution.computeMinimumVolumeIntervalWithMarginalProbability(
    0.95
)
ott.assert_almost_equal(interval.getLowerBound(), [-0.7], 1e-5, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [2.3], 1e-5, 0.0)
ott.assert_almost_equal(threshold, 0.95, 1e-5, 0.0)
levelSet, beta = distribution.computeMinimumVolumeLevelSetWithThreshold(0.95)
ott.assert_almost_equal(levelSet.getLevel(), 2.77259, 1e-5, 0.0)
assert "MinimumVolumeLevelSetEvaluation(Histogram(origin = -1.5" in str(levelSet)
ott.assert_almost_equal(beta, 0.0625, 1e-5, 0.0)
interval, beta = distribution.computeBilateralConfidenceIntervalWithMarginalProbability(
    0.95
)
ott.assert_almost_equal(interval.getLowerBound(), [-1.1], 1e-5, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [2.22], 1e-5, 0.0)
ott.assert_almost_equal(beta, 0.95, 1e-5, 0.0)
(
    interval,
    beta,
) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, False)
ott.assert_almost_equal(interval.getLowerBound(), [-1.5], 1e-5, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [2.14], 1e-5, 0.0)
ott.assert_almost_equal(beta, 0.95, 1e-5, 0.0)
(
    interval,
    beta,
) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, True)
ott.assert_almost_equal(interval.getLowerBound(), [-0.7], 1e-5, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [2.3], 1e-5, 0.0)
ott.assert_almost_equal(beta, 0.95, 1e-5, 0.0)

mean = distribution.getMean()
ott.assert_almost_equal(mean, [0.858125], 1e-5, 0.0)
covariance = distribution.getCovariance()
ott.assert_almost_equal(covariance, ot.CovarianceMatrix(1, [0.720205]), 1e-5, 0.0)
parameters = distribution.getParametersCollection()
assert len(parameters) == 1
ott.assert_almost_equal(
    parameters[0], [-1.5, 1.0, 0.0625, 0.7, 0.1875, 1.2, 0.4375, 0.9, 0.3125], 1e-12, 0.0
)
assert distribution.getStandardRepresentative().getImplementation().getClassName() == "Histogram"

ot.Log.Show(ot.Log.TRACE)
validation = ott.DistributionValidation(distribution)
# Width/height changes renormalize heights, so the parameter roundtrip holds to ~1e-2;
# flat density pieces over-cover level sets
validation.setParameterTolerance(5e-2)
validation.setDomainTolerance(5e-2)
validation.skipPDFAtLowerBound()  # the density is the first bin height at the finite lower bound
validation.run()

# Density at the support edges: first and last bin heights
lower = distribution.getRange().getLowerBound()[0]
upper = distribution.getRange().getUpperBound()[0]
ott.assert_almost_equal(distribution.computePDF(lower), 0.0625, 1e-12, 0.0)
ott.assert_almost_equal(distribution.computePDF(upper), 0.3125, 1e-12, 0.0)
assert distribution.computePDF(lower) > 0.0
assert distribution.computePDF(upper) > 0.0

#! /usr/bin/env python

import math
import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Instantiate one distribution object
distribution = ot.VonMises(-0.5, 1.5)
assert distribution.getClassName() == "VonMises"

# Is this distribution elliptical ?
assert not distribution.isElliptical()

# Is this distribution continuous ?
assert distribution.isContinuous()

# Test for realization of distribution
oneRealization = distribution.getRealization()
ott.assert_almost_equal(oneRealization, [0.492446], 1e-5, 0.0)
ott.assert_almost_equal(oneRealization.getDimension(), 1)

# Define a point
point = ot.Point(distribution.getDimension(), 1.0)
ott.assert_almost_equal(point, [1.0], 0.0, 0.0)

# Show PDF and CDF of point
eps = 1e-5
DDF = distribution.computeDDF(point)
ott.assert_almost_equal(DDF, [-0.160799], 1e-5, 0.0)
LPDF = distribution.computeLogPDF(point)
ott.assert_almost_equal(LPDF, -2.23055863162, 1e-9, 0.0)
PDF = distribution.computePDF(point)
ott.assert_almost_equal(PDF, 0.107468, 1e-6, 1e-6)
ott.assert_almost_equal(PDF, ot.math.exp(LPDF))
# DDF vs finite difference
ddfFD = (
    distribution.computePDF(point + ot.Point(1, eps))
    - distribution.computePDF(point + ot.Point(1, -eps))
) / (2.0 * eps)
ott.assert_almost_equal(DDF[0], ddfFD, 1e-4, 1e-4)

CDF = distribution.computeCDF(point)
ott.assert_almost_equal(CDF, 0.927414932933, 1e-9, 0.0)
CCDF = distribution.computeComplementaryCDF(point)
ott.assert_almost_equal(CCDF, 0.0725850670669, 1e-9, 0.0)
ott.assert_almost_equal(CCDF, 1.0 - CDF)
Survival = distribution.computeSurvivalFunction(point)
ott.assert_almost_equal(Survival, 0.0725850670669, 1e-9, 0.0)
ott.assert_almost_equal(Survival, 1.0 - CDF)
CF = distribution.computeCharacteristicFunction(point[0])
ott.assert_almost_equal([CF.real, CF.imag], [0.523156, -0.285801], 1e-6, 1e-6)
ott.assert_almost_equal(distribution.computeCharacteristicFunction(0.0), 1.0 + 0.0j)
LCF = distribution.computeLogCharacteristicFunction(point[0])
ott.assert_almost_equal([LCF.real, LCF.imag], [-0.517291, -0.5], 1e-6, 1e-6)
ott.assert_almost_equal(distribution.computeLogCharacteristicFunction(0.0), 0.0 + 0.0j)
PDFgr = distribution.computePDFGradient(point)
ott.assert_almost_equal(PDFgr, [0.160799, -0.0564635], 1e-5, 0.0)
# FD check of PDF gradient
PDFgrFD = ot.Point(2)
PDFgrFD[0] = (
    ot.VonMises(distribution.getMu() + eps, distribution.getKappa()).computePDF(point)
    - ot.VonMises(distribution.getMu() - eps, distribution.getKappa()).computePDF(point)
) / (2.0 * eps)
PDFgrFD[1] = (
    ot.VonMises(distribution.getMu(), distribution.getKappa() + eps).computePDF(point)
    - ot.VonMises(distribution.getMu(), distribution.getKappa() - eps).computePDF(point)
) / (2.0 * eps)
ott.assert_almost_equal(PDFgr, PDFgrFD, 1e-4, 1e-4)

CDFgr = distribution.computeCDFGradient(point)
ott.assert_almost_equal(CDFgr, [-0.107468, 0.0747113], 1e-5, 0.0)
# FD check of CDF gradient
CDFgrFD = ot.Point(2)
CDFgrFD[0] = (
    ot.VonMises(distribution.getMu() + eps, distribution.getKappa()).computeCDF(point)
    - ot.VonMises(distribution.getMu() - eps, distribution.getKappa()).computeCDF(point)
) / (2.0 * eps)
CDFgrFD[1] = (
    ot.VonMises(distribution.getMu(), distribution.getKappa() + eps).computeCDF(point)
    - ot.VonMises(distribution.getMu(), distribution.getKappa() - eps).computeCDF(point)
) / (2.0 * eps)
ott.assert_almost_equal(CDFgr, CDFgrFD, 1e-4, 1e-4)

quantile = distribution.computeQuantile(0.95)
ott.assert_almost_equal(quantile, [1.25235], 1e-5, 0.0)
ott.assert_almost_equal(distribution.computeCDF(quantile), 0.95, 1e-9, 0.0)
# Get 95% survival function
inverseSurvival = ot.Point(distribution.computeInverseSurvivalFunction(0.95))
ott.assert_almost_equal(inverseSurvival, [-2.25235], 1e-5, 0.0)
ott.assert_almost_equal(
    distribution.computeSurvivalFunction(inverseSurvival), 0.95, 1e-6, 1e-6
)
ott.assert_almost_equal(distribution.computeEntropy(), 1.442465, 1e-6, 1e-6)

# Confidence regions
interval, threshold = distribution.computeMinimumVolumeIntervalWithMarginalProbability(
    0.95
)
ott.assert_almost_equal(interval.getLowerBound(), [-2.71399], 1e-5, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [1.71399], 1e-5, 0.0)
ott.assert_almost_equal(threshold, 0.95, 1e-5, 0.0)
levelSet, beta = distribution.computeMinimumVolumeLevelSetWithThreshold(0.95)
ott.assert_almost_equal(beta, 0.0393091, 1e-5, 0.0)
interval, beta = distribution.computeBilateralConfidenceIntervalWithMarginalProbability(
    0.95
)
ott.assert_almost_equal(interval.getLowerBound(), [-2.71399], 1e-5, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [1.71399], 1e-5, 0.0)
ott.assert_almost_equal(beta, 0.95, 1e-5, 0.0)
(
    interval,
    beta,
) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, False)
ott.assert_almost_equal(interval.getLowerBound(), [-3.64159], 1e-5, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [1.25235], 1e-5, 0.0)
ott.assert_almost_equal(beta, 0.95, 1e-5, 0.0)
(
    interval,
    beta,
) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, True)
ott.assert_almost_equal(interval.getLowerBound(), [-2.25235], 1e-5, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [2.64159], 1e-5, 0.0)
ott.assert_almost_equal(beta, 0.95, 1e-5, 0.0)

mean = distribution.getMean()
ott.assert_almost_equal(mean, [-0.5], 1e-5, 0.0)
ott.assert_almost_equal(mean[0], distribution.getCircularMean())
standardDeviation = distribution.getStandardDeviation()
ott.assert_almost_equal(standardDeviation, [1.04438], 1e-5, 0.0)
skewness = distribution.getSkewness()
ott.assert_almost_equal(skewness, [1.94923e-15], 1e-5, 1e-12)
kurtosis = distribution.getKurtosis()
ott.assert_almost_equal(kurtosis, [3.44545], 1e-5, 0.0)
covariance = distribution.getCovariance()
ott.assert_almost_equal(covariance, ot.CovarianceMatrix(1, [1.09073]), 1e-5, 0.0)
correlation = distribution.getCorrelation()
ott.assert_almost_equal(correlation, ot.CorrelationMatrix(1), 0.0, 0.0)
spearman = distribution.getSpearmanCorrelation()
ott.assert_almost_equal(spearman, ot.CorrelationMatrix(1), 0.0, 0.0)
kendall = distribution.getKendallTau()
ott.assert_almost_equal(kendall, ot.CorrelationMatrix(1), 0.0, 0.0)
parameters = distribution.getParametersCollection()
assert len(parameters) == 1
ott.assert_almost_equal(parameters[0], [-0.5, 1.5], 0.0, 0.0)
assert distribution.getStandardRepresentative().getImplementation().getClassName() == "VonMises"
ott.assert_almost_equal(distribution.getCircularMean(), distribution.getMu())
ott.assert_almost_equal(distribution.getCircularVariance(), 0.403866761169, 1e-9, 0.0)
cv = distribution.getCircularVariance()
assert cv >= 0.0
assert cv < 1.0

ot.Log.Show(ot.Log.TRACE)
validation = ott.DistributionValidation(distribution)
validation.skipPDFAtLowerBound()  # the periodic density is nonzero at the cut point
validation.run()

# Density at the cut point: periodic, nonzero on both sides
edgePDF = distribution.computePDF(-0.5 + math.pi)
ott.assert_almost_equal(edgePDF, 0.0215654143702237, 1e-12, 0.0)
ott.assert_almost_equal(edgePDF, distribution.computePDF(-0.5 - math.pi), 1e-12, 0.0)
assert edgePDF > 0.0

# Additional coverage: multiple parameter combinations
# Test with mu=0, various kappa
for mu, kappa in [(0.0, 0.1), (0.0, 1.0), (0.0, 10.0), (2.0, 5.0)]:
    dist = ot.VonMises(mu, kappa)
    # Circular mean equals mu
    ott.assert_almost_equal(dist.getCircularMean(), mu)
    # Circular variance in [0,1)
    cv = dist.getCircularVariance()
    assert cv >= 0.0
    assert cv < 1.0
    # Mean equals mu
    ott.assert_almost_equal(dist.getMean()[0], mu)
    # PDF outside support
    pi = math.pi
    ott.assert_almost_equal(dist.computePDF([mu - pi - 1.0]), 0.0)
    ott.assert_almost_equal(dist.computePDF([mu + pi + 1.0]), 0.0)
    # PDF inside support is positive
    assert dist.computePDF([mu]) > 0.0
    # CDF at boundaries
    ott.assert_almost_equal(dist.computeCDF([mu - pi - 1.0]), 0.0, 1e-12, 0.0)
    ott.assert_almost_equal(dist.computeCDF([mu + pi + 1.0]), 1.0, 1e-12, 0.0)
    # CDF inside support is in (0,1)
    assert 0.0 < dist.computeCDF([mu]) < 1.0
    # PDF mode at mu (highest density at center)
    pdfMode = dist.computePDF([mu])
    pdfOff = dist.computePDF([mu + 1.0])
    assert pdfMode > pdfOff
    # PDF normalization: integral over support ~ 1 via CDF
    ott.assert_almost_equal(dist.computeCDF([mu - pi]), 0.0, 1e-10, 0.0)
    ott.assert_almost_equal(dist.computeCDF([mu + pi]), 1.0, 1e-10, 0.0)
    # Characteristic function at 0
    ott.assert_almost_equal(dist.computeCharacteristicFunction(0.0), 1.0 + 0.0j)
    ott.assert_almost_equal(dist.computeLogCharacteristicFunction(0.0), 0.0 + 0.0j)
    # Entropy consistency
    assert dist.computeEntropy() > 0.0
    # Parameter round-trip
    param = dist.getParameter()
    dist2 = ot.VonMises(param[0], param[1])
    ott.assert_almost_equal(dist2.getParameter(), param)
    # Standard representative
    stdRep = dist.getStandardRepresentative()
    ott.assert_almost_equal(stdRep.getParameter(), dist.getParameter())

# Test setMu updates range
dist = ot.VonMises(0.0, 1.0)
ott.assert_almost_equal(dist.getRange().getLowerBound()[0], -math.pi)
ott.assert_almost_equal(dist.getRange().getUpperBound()[0], math.pi)
dist.setMu(2.5)
ott.assert_almost_equal(dist.getRange().getLowerBound()[0], 2.5 - math.pi)
ott.assert_almost_equal(dist.getRange().getUpperBound()[0], 2.5 + math.pi)

# Test setKappa updates internal state
dist.setKappa(3.0)
ott.assert_almost_equal(dist.getKappa(), 3.0)
ott.assert_almost_equal(
    dist.getCircularVariance(), 1.0 - ot.math.exp(ot.SpecFunc.DeltaLogBesselI10(3.0))
)

# Test setParameter / getParameter round-trip
dist = ot.VonMises(-1.2, 2.3)
param = dist.getParameter()
dist.setParameter(param)
ott.assert_almost_equal(dist.getMu(), -1.2)
ott.assert_almost_equal(dist.getKappa(), 2.3)

# Test getParametersCollection
pc = dist.getParametersCollection()
ott.assert_almost_equal(pc[0][0], -1.2)
ott.assert_almost_equal(pc[0][1], 2.3)

# Test invalid kappa raises
with ott.assert_raises(TypeError):
    ot.VonMises(0.0, -1.0)
with ott.assert_raises(TypeError):
    ot.VonMises(0.0, 0.0)

# Test large kappa behavior (near-normal approximation)
dist = ot.VonMises(0.0, 100.0)
ott.assert_almost_equal(dist.getCircularMean(), 0.0)
cv = dist.getCircularVariance()
assert cv < 0.1
# PDF at mode for large kappa
pdfModeLarge = dist.computePDF([0.0])
ott.assert_almost_equal(pdfModeLarge, 3.98441, 1e-5, 0.0)
assert pdfModeLarge > 0.0
# Quantiles near 0 for large kappa
q05 = dist.computeQuantile(0.05)[0]
q95 = dist.computeQuantile(0.95)[0]
assert q05 < 0.0
assert q95 > 0.0

# Test small kappa behavior (near-uniform)
dist = ot.VonMises(0.0, 0.01)
ott.assert_almost_equal(dist.getCircularMean(), 0.0)
# Near-uniform: PDF should be roughly constant ~ 1/(2*pi)
pdfCenter = dist.computePDF([0.0])
pdfEdge = dist.computePDF([math.pi - 1e-6])
assert abs(pdfCenter / pdfEdge - 1.0) < 0.03

# Test DDF consistency with PDF gradient for multiple points
dist = ot.VonMises(1.0, 2.0)
for x in [-2.0, 0.0, 1.0, 2.5]:
    pt = ot.Point([x])
    ddf = dist.computeDDF(pt)
    pdf = dist.computePDF(pt)
    ddfFD = (
        dist.computePDF(pt + ot.Point([eps])) - dist.computePDF(pt - ot.Point([eps]))
    ) / (2.0 * eps)
    ott.assert_almost_equal(ddf[0], ddfFD, 1e-4, 1e-4)

# Test round-trip: quantile then CDF
dist = ot.VonMises(0.3, 2.5)
for p in [0.01, 0.1, 0.5, 0.9, 0.99]:
    q = dist.computeQuantile(p)
    cdf_at_q = dist.computeCDF(q)
    ott.assert_almost_equal(cdf_at_q, p, 1e-5, 0.0)

#! /usr/bin/env python

import math
import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Instantiate one distribution object
distribution = ot.VonMises(-0.5, 1.5)
print("Distribution ", distribution)

# Is this distribution elliptical ?
print("Elliptical = ", distribution.isElliptical())
assert not distribution.isElliptical()

# Is this distribution continuous ?
print("Continuous = ", distribution.isContinuous())
assert distribution.isContinuous()

# Test for realization of distribution
oneRealization = distribution.getRealization()
print("oneRealization=", oneRealization)
ott.assert_almost_equal(oneRealization.getDimension(), 1)

# Define a point
point = ot.Point(distribution.getDimension(), 1.0)
print("Point= ", point)

# Show PDF and CDF of point
eps = 1e-5
DDF = distribution.computeDDF(point)
print("ddf     =", DDF)
LPDF = distribution.computeLogPDF(point)
print("log pdf= %.12g" % LPDF)
PDF = distribution.computePDF(point)
print("pdf     =%.6f" % PDF)
ott.assert_almost_equal(PDF, ot.math.exp(LPDF))
# DDF vs finite difference
ddfFD = (
    distribution.computePDF(point + ot.Point(1, eps))
    - distribution.computePDF(point + ot.Point(1, -eps))
) / (2.0 * eps)
ott.assert_almost_equal(DDF[0], ddfFD, 1e-4, 1e-4)

CDF = distribution.computeCDF(point)
print("cdf= %.12g" % CDF)
CCDF = distribution.computeComplementaryCDF(point)
print("ccdf= %.12g" % CCDF)
ott.assert_almost_equal(CCDF, 1.0 - CDF)
Survival = distribution.computeSurvivalFunction(point)
print("survival= %.12g" % Survival)
ott.assert_almost_equal(Survival, 1.0 - CDF)
CF = distribution.computeCharacteristicFunction(point[0])
print("characteristic function=(%.6g, %.6g)" % (CF.real, CF.imag))
ott.assert_almost_equal(distribution.computeCharacteristicFunction(0.0), 1.0 + 0.0j)
LCF = distribution.computeLogCharacteristicFunction(point[0])
print("log characteristic function=(%.6g, %.6g)" % (LCF.real, LCF.imag))
ott.assert_almost_equal(distribution.computeLogCharacteristicFunction(0.0), 0.0 + 0.0j)
PDFgr = distribution.computePDFGradient(point)
print("pdf gradient     =", PDFgr)
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
print("cdf gradient     =", CDFgr)
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
print("quantile=", quantile)
print("cdf(quantile)= %.12g" % distribution.computeCDF(quantile))
ott.assert_almost_equal(distribution.computeCDF(quantile), 0.95, 1e-5, 0.0)
# Get 95% survival function
inverseSurvival = ot.Point(distribution.computeInverseSurvivalFunction(0.95))
print("InverseSurvival=", repr(inverseSurvival))
print(
    "Survival(inverseSurvival)=%.6f"
    % distribution.computeSurvivalFunction(inverseSurvival)
)
ott.assert_almost_equal(
    distribution.computeSurvivalFunction(inverseSurvival), 0.95, 1e-5, 0.0
)
print("entropy=%.6f" % distribution.computeEntropy())

# Confidence regions
interval, threshold = distribution.computeMinimumVolumeIntervalWithMarginalProbability(
    0.95
)
print("Minimum volume interval=", interval)
print("threshold=", ot.Point(1, threshold))
levelSet, beta = distribution.computeMinimumVolumeLevelSetWithThreshold(0.95)
print("Minimum volume level set=", levelSet)
print("beta=", ot.Point(1, beta))
interval, beta = distribution.computeBilateralConfidenceIntervalWithMarginalProbability(
    0.95
)
print("Bilateral confidence interval=", interval)
print("beta=", ot.Point(1, beta))
(
    interval,
    beta,
) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, False)
print("Unilateral confidence interval (lower tail)=", interval)
print("beta=", ot.Point(1, beta))
(
    interval,
    beta,
) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, True)
print("Unilateral confidence interval (upper tail)=", interval)
print("beta=", ot.Point(1, beta))

mean = distribution.getMean()
print("mean=", mean)
ott.assert_almost_equal(mean[0], distribution.getCircularMean())
standardDeviation = distribution.getStandardDeviation()
print("standard deviation=", standardDeviation)
skewness = distribution.getSkewness()
print("skewness=", skewness)
kurtosis = distribution.getKurtosis()
print("kurtosis=", kurtosis)
covariance = distribution.getCovariance()
print("covariance=", covariance)
correlation = distribution.getCorrelation()
print("correlation=", correlation)
spearman = distribution.getSpearmanCorrelation()
print("spearman=", spearman)
kendall = distribution.getKendallTau()
print("kendall=", kendall)
parameters = distribution.getParametersCollection()
print("parameters=", parameters)
print("Standard representative=", distribution.getStandardRepresentative())
print("Circular mean=", distribution.getCircularMean())
print("Circular variance= %.12g" % distribution.getCircularVariance())
ott.assert_almost_equal(distribution.getCircularMean(), distribution.getMu())
cv = distribution.getCircularVariance()
assert cv >= 0.0
assert cv < 1.0

ot.Log.Show(ot.Log.TRACE)
validation = ott.DistributionValidation(distribution)
validation.run()

# Additional coverage: multiple parameter combinations
print("")
print("Additional tests")

# Test with mu=0, various kappa
for mu, kappa in [(0.0, 0.1), (0.0, 1.0), (0.0, 10.0), (2.0, 5.0)]:
    dist = ot.VonMises(mu, kappa)
    print("VonMises(mu=%s, kappa=%s)" % (mu, kappa))
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
print("setMu range OK")

# Test setKappa updates internal state
dist.setKappa(3.0)
ott.assert_almost_equal(dist.getKappa(), 3.0)
ott.assert_almost_equal(
    dist.getCircularVariance(), 1.0 - ot.math.exp(ot.SpecFunc.DeltaLogBesselI10(3.0))
)
print("setKappa OK")

# Test setParameter / getParameter round-trip
dist = ot.VonMises(-1.2, 2.3)
param = dist.getParameter()
dist.setParameter(param)
ott.assert_almost_equal(dist.getMu(), -1.2)
ott.assert_almost_equal(dist.getKappa(), 2.3)
print("setParameter round-trip OK")

# Test getParametersCollection
pc = dist.getParametersCollection()
ott.assert_almost_equal(pc[0][0], -1.2)
ott.assert_almost_equal(pc[0][1], 2.3)
print("getParametersCollection OK")

# Test invalid kappa raises
with ott.assert_raises(TypeError):
    ot.VonMises(0.0, -1.0)
with ott.assert_raises(TypeError):
    ot.VonMises(0.0, 0.0)
print("invalid kappa raises OK")

# Test large kappa behavior (near-normal approximation)
dist = ot.VonMises(0.0, 100.0)
ott.assert_almost_equal(dist.getCircularMean(), 0.0)
cv = dist.getCircularVariance()
assert cv < 0.1
# PDF at mode for large kappa
pdfModeLarge = dist.computePDF([0.0])
print("Large kappa PDF(0)=%.6g" % pdfModeLarge)
assert pdfModeLarge > 0.0
# Quantiles near 0 for large kappa
q05 = dist.computeQuantile(0.05)[0]
q95 = dist.computeQuantile(0.95)[0]
assert q05 < 0.0
assert q95 > 0.0
print("Large kappa quantile OK")

# Test small kappa behavior (near-uniform)
dist = ot.VonMises(0.0, 0.01)
ott.assert_almost_equal(dist.getCircularMean(), 0.0)
# Near-uniform: PDF should be roughly constant ~ 1/(2*pi)
pdfCenter = dist.computePDF([0.0])
pdfEdge = dist.computePDF([math.pi - 1e-6])
assert abs(pdfCenter / pdfEdge - 1.0) < 0.03
print("Small kappa uniformity OK")

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
print("DDF gradient FD OK")

# Test round-trip: quantile then CDF
dist = ot.VonMises(0.3, 2.5)
for p in [0.01, 0.1, 0.5, 0.9, 0.99]:
    q = dist.computeQuantile(p)
    cdf_at_q = dist.computeCDF(q)
    ott.assert_almost_equal(cdf_at_q, p, 1e-5, 0.0)
print("Quantile/CDF round-trip OK")

print("All tests passed")

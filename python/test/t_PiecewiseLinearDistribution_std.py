#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott
import copy
import math
import os
import pickle
import tempfile
from io import BytesIO

ot.TESTPREAMBLE()

# ---------------------------------------------------------------------------
# 1. Default constructor
# ---------------------------------------------------------------------------
dist0 = otexp.PiecewiseLinearDistribution()
ott.assert_almost_equal(dist0.getX(), ot.Point([-1.0, 1.0]))
ott.assert_almost_equal(dist0.getY(), ot.Point([0.5, 0.5]))
ott.assert_almost_equal(dist0.getDimension(), 1)
ott.assert_almost_equal(dist0.computePDF(0.0), 0.5)
ott.assert_almost_equal(dist0.computeCDF(0.0), 0.5)
print("Default constructor OK")

# ---------------------------------------------------------------------------
# 2. Parameterized constructor -- trapezoid-like shape
# ---------------------------------------------------------------------------
x = ot.Point([0.0, 1.0, 2.0])
y = ot.Point([0.1, 1.0, 0.1])
distribution = otexp.PiecewiseLinearDistribution(x, y)
print("Distribution ", repr(distribution))
print("Distribution ", distribution)

# getX / getY
ott.assert_almost_equal(distribution.getX(), x)
ott.assert_almost_equal(distribution.getY(), y)
print("getX/getY OK")

# ---------------------------------------------------------------------------
# 3. PDF -- scalar and Point overloads, at knots, midpoints, outside support
# ---------------------------------------------------------------------------
ott.assert_almost_equal(distribution.computePDF(0.0), 0.1 / 1.1)
ott.assert_almost_equal(distribution.computePDF(1.0), 1.0 / 1.1)
ott.assert_almost_equal(distribution.computePDF(2.0), 0.1 / 1.1)

# midpoint of first segment
ott.assert_almost_equal(distribution.computePDF(0.5), 0.55 / 1.1)

# Point overload
ott.assert_almost_equal(distribution.computePDF(ot.Point([0.5])), 0.55 / 1.1)

# outside support
ott.assert_almost_equal(distribution.computePDF(-1.0), 0.0)
ott.assert_almost_equal(distribution.computePDF(3.0), 0.0)
print("PDF OK")

# ---------------------------------------------------------------------------
# 4. CDF -- scalar and Point overloads, at knots, midpoints, boundaries
# ---------------------------------------------------------------------------
ott.assert_almost_equal(distribution.computeCDF(0.0), 0.0)
ott.assert_almost_equal(distribution.computeCDF(2.0), 1.0)

# CDF at midpoint of first segment: integral of 0.1 + 0.9*t from 0 to 0.5
# = 0.5*(0.1 + 0.55) = 0.325, normalized by 1.1 -> 0.25/1.1
cdf_half_0 = distribution.computeCDF(0.5)
ott.assert_almost_equal(cdf_half_0, 0.5 * (0.1 / 1.1 + 0.55 / 1.1) * 0.5)

# Point overload
ott.assert_almost_equal(distribution.computeCDF(ot.Point([0.5])), cdf_half_0)
print("CDF OK")

# ---------------------------------------------------------------------------
# 5. DDF -- at midpoints, knots, outside support
# ---------------------------------------------------------------------------
ddf_mid = distribution.computeDDF(ot.Point([0.5]))
# DDF is the slope of the normalized PDF: (yNorm[1] - yNorm[0]) / (x[1] - x[0])
expected_slope = (1.0 / 1.1 - 0.1 / 1.1) / 1.0
ott.assert_almost_equal(ddf_mid[0], expected_slope)

# DDF outside support
ddf_neg = distribution.computeDDF(ot.Point([-1.0]))
ott.assert_almost_equal(ddf_neg[0], 0.0)
ddf_big = distribution.computeDDF(ot.Point([3.0]))
ott.assert_almost_equal(ddf_big[0], 0.0)
print("DDF OK")

# ---------------------------------------------------------------------------
# 6. Quantile -- default tail, tail=True, boundary probabilities, round-trip
# ---------------------------------------------------------------------------
q50 = distribution.computeQuantile(0.5)
ott.assert_almost_equal(q50[0], distribution.computeScalarQuantile(0.5))

# tail=True
q95_tail = distribution.computeScalarQuantile(0.05, True)
q95 = distribution.computeScalarQuantile(0.95)
ott.assert_almost_equal(q95_tail, q95)

# boundary values
ott.assert_almost_equal(distribution.computeScalarQuantile(0.0), 0.0)
ott.assert_almost_equal(distribution.computeScalarQuantile(1.0), 2.0)

# round-trip: CDF(quantile(p)) ~ p
for prob in [0.1, 0.25, 0.5, 0.75, 0.9]:
    q = distribution.computeScalarQuantile(prob)
    cdf = distribution.computeCDF(q)
    ott.assert_almost_equal(cdf, prob)

# same round-trip on non-unit segment widths (dist0 has dx=2)
for prob in [0.1, 0.25, 0.5, 0.75, 0.9]:
    q = dist0.computeScalarQuantile(prob)
    ott.assert_almost_equal(dist0.computeCDF(q), prob)
print("Quantile OK")

# ---------------------------------------------------------------------------
# 7. Moments -- with assertions
# ---------------------------------------------------------------------------
mean = distribution.getMean()
assert mean[0] >= 0.0
assert mean[0] <= 2.0
cov = distribution.getCovariance()
assert cov[0, 0] > 0.0
sd = distribution.getStandardDeviation()
ott.assert_almost_equal(sd[0], math.sqrt(cov[0, 0]))
skewness = distribution.getSkewness()
kurtosis = distribution.getKurtosis()
print("Moments OK")

# entropy
entropy = distribution.computeEntropy()
assert entropy > 0.0

# roughness
roughness = distribution.getRoughness()
assert roughness > 0.0

# singularities -- interior knots for 3-point distribution
singularities = distribution.getSingularities()
ott.assert_almost_equal(singularities, ot.Point([1.0]))
print("Entropy/Roughness/Singularities OK")

# ---------------------------------------------------------------------------
# 8. Realization and sample
# ---------------------------------------------------------------------------
oneRealization = distribution.getRealization()
assert len(oneRealization) == 1
assert oneRealization[0] >= 0.0
assert oneRealization[0] <= 2.0

sample = distribution.getSample(100)
ott.assert_almost_equal(sample.getSize(), 100)
ott.assert_almost_equal(sample.getDimension(), 1)
print("Realization/Sample OK")

# ---------------------------------------------------------------------------
# 9. Parameters -- getParameter, setParameter, getParameterDescription
# ---------------------------------------------------------------------------
params = distribution.getParameter()
ott.assert_almost_equal(params, ot.Point([0.0, 1.0, 2.0, 0.1, 1.0, 0.1]))

paramDesc = distribution.getParameterDescription()
assert len(paramDesc) == 6
assert paramDesc[0] == "x0"
assert paramDesc[3] == "y0"

# setParameter round-trip
newParams = ot.Point([0.0, 1.5, 3.0, 0.2, 1.0, 0.2])
distribution2 = otexp.PiecewiseLinearDistribution(
    ot.Point([0.0, 1.5, 3.0]), ot.Point([0.2, 1.0, 0.2])
)
distribution2.setParameter(newParams)
ott.assert_almost_equal(distribution2.getParameter(), newParams)
print("Parameters OK")

# ---------------------------------------------------------------------------
# 10. setX / setY accessors
# ---------------------------------------------------------------------------
dist_set = otexp.PiecewiseLinearDistribution(x, y)

# setY -- change ordinates
new_y = ot.Point([0.2, 0.8, 0.2])
dist_set.setY(new_y)
ott.assert_almost_equal(dist_set.getY(), new_y)
# after normalization: total integral = 0.5*(0.2+0.8)*1 + 0.5*(0.8+0.2)*1 = 1.0
# so yNorm = y (already normalized), PDF at knot is the normalized value
ott.assert_almost_equal(dist_set.computePDF(1.0), 0.8)

# setX -- change abscissae
new_x = ot.Point([0.0, 2.0, 4.0])
dist_set.setX(new_x)
ott.assert_almost_equal(dist_set.getX(), new_x)
print("setX/setY OK")

# ---------------------------------------------------------------------------
# 11. setX / setY error handling
# ---------------------------------------------------------------------------
with ott.assert_raises(TypeError):
    dist_set.setX(ot.Point([0.0]))  # wrong size
print("OK: setX wrong size thrown")

with ott.assert_raises(TypeError):
    dist_set.setX(ot.Point([2.0, 0.0, 4.0]))  # not increasing
print("OK: setX non-increasing thrown")

with ott.assert_raises(TypeError):
    dist_set.setY(ot.Point([0.0]))  # wrong size
print("OK: setY wrong size thrown")

with ott.assert_raises(TypeError):
    dist_set.setY(ot.Point([0.2, -0.1, 0.2]))  # negative y
print("OK: setY negative thrown")

# ---------------------------------------------------------------------------
# 12. Probability computations
# ---------------------------------------------------------------------------
# Full range
interval_full = ot.Interval([0.0], [2.0])
prob_full = distribution.computeProbability(interval_full)
ott.assert_almost_equal(prob_full, 1.0)

# Sub-interval
interval = ot.Interval([0.5], [1.5])
prob = distribution.computeProbability(interval)
assert prob > 0.0 and prob < 1.0

# Empty interval
interval_empty = ot.Interval([1.0], [0.5])
prob_empty = distribution.computeProbability(interval_empty)
ott.assert_almost_equal(prob_empty, 0.0)

# Zero-width interval (point)
interval_point = ot.Interval([1.0], [1.0])
prob_point = distribution.computeProbability(interval_point)
ott.assert_almost_equal(prob_point, 0.0)
print("Probability OK")

# ---------------------------------------------------------------------------
# 13. Complementary CDF, Survival Function, Inverse Survival Function
# ---------------------------------------------------------------------------
ccdf = distribution.computeComplementaryCDF(ot.Point([1.0]))
ott.assert_almost_equal(ccdf, 1.0 - distribution.computeCDF(1.0))

sf = distribution.computeSurvivalFunction(ot.Point([1.0]))
ott.assert_almost_equal(sf, 1.0 - distribution.computeCDF(1.0))

isf = distribution.computeInverseSurvivalFunction(0.5)
ott.assert_almost_equal(distribution.computeSurvivalFunction(isf), 0.5)
print("CCDF/SF/ISF OK")

# ---------------------------------------------------------------------------
# 14. Confidence intervals
# ---------------------------------------------------------------------------
# Minimum volume interval
interval_mvi, threshold_mvi = distribution.computeMinimumVolumeIntervalWithMarginalProbability(0.95)
assert (threshold_mvi > 0.0)

# Minimum volume level set
levelSet, beta = distribution.computeMinimumVolumeLevelSetWithThreshold(0.95)
assert (beta > 0.0)

# Bilateral confidence interval
interval_bci, beta_bci = distribution.computeBilateralConfidenceIntervalWithMarginalProbability(0.95)
assert (beta_bci > 0.0)

# Unilateral confidence interval -- lower tail
interval_uni_lo, beta_uni_lo = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, False)
assert (beta_uni_lo > 0.0)

# Unilateral confidence interval -- upper tail
interval_uni_hi, beta_uni_hi = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, True)
assert (beta_uni_hi > 0.0)
print("Confidence intervals OK")

# ---------------------------------------------------------------------------
# 15. Equality and inequality
# ---------------------------------------------------------------------------
distribution3 = otexp.PiecewiseLinearDistribution(x, y)
assert (distribution == distribution3)
assert (not (distribution != distribution3))

# Different distribution
distribution4 = otexp.PiecewiseLinearDistribution(
    ot.Point([0.0, 1.5, 3.0]), ot.Point([0.1, 1.0, 0.1])
)
assert (not (distribution == distribution4))
assert (distribution != distribution4)
print("Equality/Inequality OK")

# ---------------------------------------------------------------------------
# 16. Copy
# ---------------------------------------------------------------------------
dist_clone = copy.copy(distribution)
ott.assert_almost_equal(dist_clone.getX(), distribution.getX())
ott.assert_almost_equal(dist_clone.getY(), distribution.getY())
print("Copy OK")

# ---------------------------------------------------------------------------
# 17. Serialization via pickle
# ---------------------------------------------------------------------------
buf = BytesIO()
pickle.dump(distribution, buf)
buf.seek(0)
dist_loaded = pickle.load(buf)
ott.assert_almost_equal(dist_loaded.getX(), distribution.getX())
ott.assert_almost_equal(dist_loaded.getY(), distribution.getY())
ott.assert_almost_equal(dist_loaded.computePDF(1.0), distribution.computePDF(1.0))
ott.assert_almost_equal(dist_loaded.computeCDF(1.0), distribution.computeCDF(1.0))
print("Pickle save/load OK")

# ---------------------------------------------------------------------------
# 18. Serialization via Study (XML)
# ---------------------------------------------------------------------------
with tempfile.NamedTemporaryFile(suffix=".xml", delete=False) as tmpfile_f:
    tmpfile = tmpfile_f.name
try:
    study = ot.Study()
    study.setStorageManager(ot.XMLStorageManager(tmpfile))
    study.add("dist", distribution)
    study.save()

    study2 = ot.Study()
    study2.setStorageManager(ot.XMLStorageManager(tmpfile))
    study2.load()
    dist_xml = otexp.PiecewiseLinearDistribution()
    study2.fillObject("dist", dist_xml)
    ott.assert_almost_equal(dist_xml.getX(), distribution.getX())
    ott.assert_almost_equal(dist_xml.getY(), distribution.getY())
    ott.assert_almost_equal(dist_xml.computePDF(1.0), distribution.computePDF(1.0))
    print("Study save/load OK")
finally:
    if os.path.exists(tmpfile):
        os.remove(tmpfile)

# ---------------------------------------------------------------------------
# 19. Different distribution shapes
# ---------------------------------------------------------------------------
# Uniform (2-point): x=[0,1], y=[1,1]
dist_uni = otexp.PiecewiseLinearDistribution(ot.Point([0.0, 1.0]), ot.Point([1.0, 1.0]))
ott.assert_almost_equal(dist_uni.computePDF(0.5), 1.0)
ott.assert_almost_equal(dist_uni.computeCDF(0.0), 0.0)
ott.assert_almost_equal(dist_uni.computeCDF(1.0), 1.0)
ott.assert_almost_equal(dist_uni.computeScalarQuantile(0.5), 0.5)
ott.assert_almost_equal(dist_uni.getMean()[0], 0.5)
ott.assert_almost_equal(dist_uni.getCovariance()[0, 0], 1.0 / 12.0)
print("Uniform shape OK")

# Triangular: x=[0,1,2], y=[0,1,0]
dist_tri = otexp.PiecewiseLinearDistribution(ot.Point([0.0, 1.0, 2.0]), ot.Point([0.0, 1.0, 0.0]))
ott.assert_almost_equal(dist_tri.computePDF(1.0), 1.0)
ott.assert_almost_equal(dist_tri.computeCDF(0.0), 0.0)
ott.assert_almost_equal(dist_tri.computeCDF(2.0), 1.0)
ott.assert_almost_equal(dist_tri.getMean()[0], 1.0)
print("Triangular shape OK")

# Multi-segment: 4 points
x4 = ot.Point([0.0, 1.0, 2.0, 3.0])
y4 = ot.Point([0.1, 0.5, 0.3, 0.1])
dist4 = otexp.PiecewiseLinearDistribution(x4, y4)
ott.assert_almost_equal(dist4.computeCDF(0.0), 0.0)
ott.assert_almost_equal(dist4.computeCDF(3.0), 1.0)
sing4 = dist4.getSingularities()
ott.assert_almost_equal(sing4, ot.Point([1.0, 2.0]))
print("Multi-segment shape OK")

# Asymmetric segments
x_asym = ot.Point([0.0, 1.0, 3.0, 5.0])
y_asym = ot.Point([0.1, 0.5, 0.3, 0.05])
dist_asym = otexp.PiecewiseLinearDistribution(x_asym, y_asym)
ott.assert_almost_equal(dist_asym.computeCDF(0.0), 0.0)
ott.assert_almost_equal(dist_asym.computeCDF(5.0), 1.0)
for prob in [0.1, 0.25, 0.5, 0.75, 0.9]:
    q = dist_asym.computeScalarQuantile(prob)
    ott.assert_almost_equal(dist_asym.computeCDF(q), prob)
print("Asymmetric shape OK")

# Interior zero: y touches zero at an interior knot
x_zero = ot.Point([0.0, 1.0, 2.0, 3.0])
y_zero = ot.Point([0.5, 0.0, 1.0, 0.5])
dist_zero = otexp.PiecewiseLinearDistribution(x_zero, y_zero)
ott.assert_almost_equal(dist_zero.computePDF(1.0), 0.0)
ott.assert_almost_equal(dist_zero.computeCDF(3.0), 1.0)
print("Interior zero shape OK")

# 2-point distribution -- singularities should be empty
sing2 = dist_uni.getSingularities()
ott.assert_almost_equal(sing2, ot.Point())
print("2-point singularities OK")

# ---------------------------------------------------------------------------
# 20. Invalid inputs
# ---------------------------------------------------------------------------
with ott.assert_raises(TypeError):
    otexp.PiecewiseLinearDistribution(ot.Point([0.0]), ot.Point([1.0]))
print("OK: thrown for x with 1 element")

with ott.assert_raises(TypeError):
    otexp.PiecewiseLinearDistribution(ot.Point([0.0, 1.0]), ot.Point([1.0]))
print("OK: thrown for mismatched sizes")

with ott.assert_raises(TypeError):
    otexp.PiecewiseLinearDistribution(ot.Point([1.0, 0.0]), ot.Point([1.0, 1.0]))
print("OK: thrown for non-increasing x")

with ott.assert_raises(TypeError):
    otexp.PiecewiseLinearDistribution(ot.Point([0.0, 1.0]), ot.Point([-1.0, 1.0]))
print("OK: thrown for negative y")

with ott.assert_raises(TypeError):
    otexp.PiecewiseLinearDistribution(ot.Point([0.0, 1.0]), ot.Point([0.0, 0.0]))
print("OK: thrown for all-zero y")

# setParameter with wrong size
with ott.assert_raises(TypeError):
    dist0.setParameter(ot.Point([1.0, 2.0]))
print("OK: setParameter wrong size thrown")

# computeScalarQuantile with out-of-range probability
with ott.assert_raises(TypeError):
    dist0.computeScalarQuantile(-0.1)
with ott.assert_raises(TypeError):
    dist0.computeScalarQuantile(1.1)
print("OK: quantile out-of-range thrown")

# ---------------------------------------------------------------------------
# 21. CDF-PDF numerical consistency via finite differences
# ---------------------------------------------------------------------------
eps = 1e-8
for test_x in [0.3, 0.7, 1.0, 1.5]:
    pdf_val = distribution.computePDF(test_x)
    cdf_deriv = (distribution.computeCDF(test_x + eps) - distribution.computeCDF(test_x - eps)) / (2.0 * eps)
    ott.assert_almost_equal(pdf_val, cdf_deriv, 1e-6, 1e-6)
print("CDF-PDF consistency OK")

# ---------------------------------------------------------------------------
# 22. Distribution properties
# ---------------------------------------------------------------------------
ott.assert_almost_equal(distribution.getDimension(), 1)
assert (distribution.isContinuous())
assert (not distribution.isDiscrete())
assert (not distribution.isElliptical())
assert (not distribution.isCopula())

range_ = distribution.getRange()
ott.assert_almost_equal(range_.getLowerBound()[0], 0.0)
ott.assert_almost_equal(range_.getUpperBound()[0], 2.0)
print("Distribution properties OK")

# ---------------------------------------------------------------------------
# 23. Draw methods (just verify they don't throw)
# ---------------------------------------------------------------------------
graph_pdf = distribution.drawPDF()
graph_cdf = distribution.drawCDF()
graph_q = distribution.drawQuantile()
print("Draw methods OK")

# ---------------------------------------------------------------------------
# 24. LogPDF (verify log(PDF) == logPDF)
# ---------------------------------------------------------------------------
for test_x in [0.0, 0.5, 1.0, 1.5, 2.0]:
    pdf_val = distribution.computePDF(test_x)
    logpdf_val = distribution.computeLogPDF(test_x)
    if pdf_val > 0.0:
        ott.assert_almost_equal(logpdf_val, math.log(pdf_val))
    else:
        ott.assert_almost_equal(logpdf_val, float("-inf"))
print("LogPDF OK")

# ---------------------------------------------------------------------------
# 25. Point overload dimension checks
# ---------------------------------------------------------------------------
with ott.assert_raises(TypeError):
    distribution.computePDF(ot.Point([1.0, 2.0]))
print("OK: computePDF(Point) wrong dimension thrown")

with ott.assert_raises(TypeError):
    distribution.computeCDF(ot.Point([1.0, 2.0]))
print("OK: computeCDF(Point) wrong dimension thrown")

with ott.assert_raises(TypeError):
    distribution.computeDDF(ot.Point([1.0, 2.0]))
print("OK: computeDDF(Point) wrong dimension thrown")

# ---------------------------------------------------------------------------
# 26. setY all-zero guard after construction
# ---------------------------------------------------------------------------
dist_test = otexp.PiecewiseLinearDistribution(x, y)
with ott.assert_raises(TypeError):
    dist_test.setY(ot.Point([0.0, 0.0, 0.0]))
print("OK: setY all-zero after construction thrown")

# ---------------------------------------------------------------------------
# 27. Validation -- comprehensive automated tests
# ---------------------------------------------------------------------------
print("Running DistributionValidation...")
ot.Log.Show(ot.Log.TRACE)
validation = ott.DistributionValidation(distribution)
validation.run()

print("All tests passed!")

#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()


# Instantiate one distribution object
distribution = ot.Multinomial(5, ot.Point(3, 0.25))
print("Distribution ", repr(distribution))
print("Distribution ", distribution)

# Is this distribution elliptical ?
print("Elliptical = ", distribution.isElliptical())
assert not distribution.isElliptical()

# Is this distribution continuous ?
print("Continuous = ", distribution.isContinuous())

# Test for realization of distribution
oneRealization = distribution.getRealization()
print("oneRealization=", oneRealization)

print("support=\n" + str(distribution.getSupport()))
interval = ot.Interval(
    ot.Point(distribution.getDimension(), 1.0),
    ot.Point(distribution.getDimension(), 3.0),
)
print(
    "support restricted to the interval=\n"
    + str(interval)
    + " gives=\n"
    + str(distribution.getSupport(interval))
)

# Define a point
point = ot.Point(distribution.getDimension(), 1.0)
print("Point= ", repr(point))

# Show PDF and CDF at point
LPDF = distribution.computeLogPDF(point)
print("log pdf=%.6f" % LPDF)
PDF = distribution.computePDF(point)
print("pdf     =%.6f" % PDF)
CDF = distribution.computeCDF(point)
print("cdf=%.5f" % CDF)
proba = distribution.computeProbability(
    ot.Interval(
        [i for i in range(distribution.getDimension())],
        [i + 1.0 for i in range(distribution.getDimension())],
    )
)
print("probability=%.5f" % proba)
quantile = distribution.computeQuantile(0.95)
print("quantile=", repr(quantile))
print("cdf(quantile)= %.6f" % distribution.computeCDF(quantile))
print("entropy=%.6f" % distribution.computeEntropy())

mean = distribution.getMean()
print("mean=", repr(mean))
covariance = distribution.getCovariance()
print("covariance=", repr(covariance))
parameters = distribution.getParametersCollection()
print("parameters=", repr(parameters))
parameter = distribution.getParameter()
print("parameter=", repr(parameter))
print("parameterDesc=", distribution.getParameterDescription())
distribution.setParameter(parameter)

ot.Log.Show(ot.Log.TRACE)
validation = ott.DistributionValidation(distribution)
validation.skipMoments()  # slow
validation.skipCorrelation()  # slow
validation.run()


# Test conditional PDF/CDF/quantile with correct denominator (1.0 - sumP)
# For Multinomial(5, [0.25,0.25,0.25]), X1|X0=1 ~ Binomial(4, 0.25/0.75) = Binomial(4, 1/3)
cond_pdf = distribution.computeConditionalPDF(2.0, [1.0])
cond_pdf_ref = ot.Binomial(4, 1.0 / 3.0).computePDF(2.0)
ott.assert_almost_equal(cond_pdf, cond_pdf_ref)

cond_cdf = distribution.computeConditionalCDF(2.0, [1.0])
cond_cdf_ref = ot.Binomial(4, 1.0 / 3.0).computeCDF(2.0)
ott.assert_almost_equal(cond_cdf, cond_cdf_ref)

cond_quant = distribution.computeConditionalQuantile(0.5, [1.0])
cond_quant_ref = ot.Binomial(4, 1.0 / 3.0).computeQuantile(0.5)[0]
ott.assert_almost_equal(cond_quant, cond_quant_ref)

# Conditional with 2 conditioning variables
# X2|X0=1,X1=1 ~ Binomial(3, 0.25/0.5) = Binomial(3, 0.5)
cond_pdf2 = distribution.computeConditionalPDF(1.0, [1.0, 1.0])
cond_pdf2_ref = ot.Binomial(3, 0.25 / 0.5).computePDF(1.0)
ott.assert_almost_equal(cond_pdf2, cond_pdf2_ref)

# Test survival function should be P(X > point), not P(X >= point)
# For 1D this is equivalent to 1 - CDF
dist_1d = ot.Multinomial(5, [0.5])
for k in range(6):
    s = dist_1d.computeSurvivalFunction([k])
    c = dist_1d.computeCDF([k])
    # S(k) = P(X > k) = 1 - P(X <= k) = 1 - CDF(k)
    ott.assert_almost_equal(s, 1.0 - c)
    # Also verify that S(k-0.5) = 1 - CDF(k-0.5) (non-integer)
    s_half = dist_1d.computeSurvivalFunction([k - 0.5])
    c_half = dist_1d.computeCDF([k - 0.5])
    ott.assert_almost_equal(s_half, 1.0 - c_half)

# For multivariate, S(point) = P(X > point) for each component
# Verify that S(point) excludes the point itself by comparing with probability
surv_int = distribution.computeSurvivalFunction([2.0, 2.0, 2.0])
# S(2,2,2) = P(X > (2,2,2)) = P(X >= (3,3,3))
surv_int_ref = distribution.computeProbability(
    ot.Interval([3.0, 3.0, 3.0], [5.0, 5.0, 5.0])
)
ott.assert_almost_equal(surv_int, surv_int_ref)
# Verify that S(2.5,2.5,2.5) = P(X >= (3,3,3)) too (since X > 2.5 means X >= 3)
surv_nonint = distribution.computeSurvivalFunction([2.5, 2.5, 2.5])
ott.assert_almost_equal(surv_nonint, surv_int_ref)

# Test PDF when sumP_ == 0 (all zero probabilities)
# Should not produce NaN
dist_allzero = ot.Multinomial(3, [0.0, 0.0, 0.0])
pdf_allzero = dist_allzero.computePDF([0.0, 0.0, 0.0])
ott.assert_almost_equal(pdf_allzero, 1.0)
pdf_allzero_off = dist_allzero.computePDF([1.0, 0.0, 0.0])
ott.assert_almost_equal(pdf_allzero_off, 0.0)

# Test CDF with slightly negative point (within epsilon tolerance)
# Should not produce UB or garbage
cdf_neg = distribution.computeCDF([-1e-14, 0.0, 0.0])
ott.assert_almost_equal(cdf_neg, 0.0)

# Test computeProbability for degenerate p=[1.0] case
dist_p1 = ot.Multinomial(5, [1.0])
prob_cover_n = dist_p1.computeProbability(ot.Interval([3.0], [6.0]))
ott.assert_almost_equal(prob_cover_n, 1.0)
prob_miss_n = dist_p1.computeProbability(ot.Interval([0.0], [4.0]))
ott.assert_almost_equal(prob_miss_n, 0.0)

# Test isElliptical always false
ott.assert_almost_equal(dist_p1.isElliptical(), False)
dist_2d = ot.Multinomial(5, [0.5, 0.5])
ott.assert_almost_equal(dist_2d.isElliptical(), False)

# Test conditional methods when sumP == 0
dist_cond_zero = ot.Multinomial(5, [0.0, 0.0, 1.0])
cond_pdf_zero = dist_cond_zero.computeConditionalPDF(0.0, [0.0])
ott.assert_almost_equal(cond_pdf_zero, 1.0)
cond_cdf_zero = dist_cond_zero.computeConditionalCDF(0.0, [0.0])
ott.assert_almost_equal(cond_cdf_zero, 1.0)
cond_quant_zero = dist_cond_zero.computeConditionalQuantile(0.5, [0.0])
ott.assert_almost_equal(cond_quant_zero, 0.0)

# Conditional methods when sumP == 1.0 (remaining prob = 0)
# For Multinomial(5, [0.5, 0.5]), X1|X0=1 ~ Binomial(4, 1.0), so all mass at X1=4
dist_cond_sum1 = ot.Multinomial(5, [0.5, 0.5])
cond_pdf_sum1 = dist_cond_sum1.computeConditionalPDF(0.0, [1.0])
ott.assert_almost_equal(cond_pdf_sum1, 0.0)
cond_pdf_sum1_nonzero = dist_cond_sum1.computeConditionalPDF(4.0, [1.0])
ott.assert_almost_equal(cond_pdf_sum1_nonzero, 1.0)

# Test sequential conditional PDF consistency
seq_pdf = distribution.computeSequentialConditionalPDF([1.0, 1.0, 1.0])
ott.assert_almost_equal(seq_pdf[0], distribution.computeConditionalPDF(1.0, []))
ott.assert_almost_equal(seq_pdf[1], distribution.computeConditionalPDF(1.0, [1.0]))
ott.assert_almost_equal(seq_pdf[2], distribution.computeConditionalPDF(1.0, [1.0, 1.0]))

# Test quantile roundtrip consistency for conditional (discrete distribution: CDF(Q(p)) >= p)
cond_quant_rt = distribution.computeConditionalQuantile(0.75, [1.0])
cond_cdf_rt = distribution.computeConditionalCDF(cond_quant_rt, [1.0])
assert cond_cdf_rt > 0.75, "Conditional quantile roundtrip failed"

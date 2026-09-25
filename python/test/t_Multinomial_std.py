#! /usr/bin/env python

import math

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Instantiate one distribution object
distribution = ot.Multinomial(5, ot.Point(3, 0.25))

assert not distribution.isElliptical()
assert not distribution.isContinuous()
assert distribution.isIntegral()

# Test for realization of distribution
oneRealization = distribution.getRealization()
assert oneRealization.getDimension() == 3

# Test for sampling
size = 10000
oneSample = distribution.getSample(size)
assert oneSample.getSize() == size

# Support
support = distribution.getSupport()
assert support.getSize() > 0
interval = ot.Interval(
    ot.Point(distribution.getDimension(), 1.0),
    ot.Point(distribution.getDimension(), 3.0),
)
restrictedSupport = distribution.getSupport(interval)
assert restrictedSupport.getSize() > 0

# PDF / CDF at a point
point = ot.Point(distribution.getDimension(), 1.0)
LPDF = distribution.computeLogPDF(point)
PDF = distribution.computePDF(point)
CDF = distribution.computeCDF(point)
assert math.isfinite(LPDF)
assert PDF >= 0.0
assert CDF >= 0.0
proba = distribution.computeProbability(
    ot.Interval(
        [i for i in range(distribution.getDimension())],
        [i + 1.0 for i in range(distribution.getDimension())],
    )
)
assert proba >= 0.0
quantile = distribution.computeQuantile(0.95)
assert quantile.getDimension() == distribution.getDimension()
assert distribution.computeCDF(quantile) >= 0.95

# Parameters
mean = distribution.getMean()
assert mean.getDimension() == distribution.getDimension()
covariance = distribution.getCovariance()
assert covariance.getDimension() == distribution.getDimension()
parameters = distribution.getParametersCollection()
parameter = distribution.getParameter()
assert parameter.getDimension() > 0
distribution.setParameter(parameter)

ot.Log.Show(ot.Log.TRACE)
validation = ott.DistributionValidation(distribution)
validation.skipMoments()
validation.skipCorrelation()
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

# Test survival function
dist_1d = ot.Multinomial(5, [0.5])
for k in range(6):
    s = dist_1d.computeSurvivalFunction([k])
    c = dist_1d.computeCDF([k])
    ott.assert_almost_equal(s, 1.0 - c)
    s_half = dist_1d.computeSurvivalFunction([k - 0.5])
    c_half = dist_1d.computeCDF([k - 0.5])
    ott.assert_almost_equal(s_half, 1.0 - c_half)

surv_int = distribution.computeSurvivalFunction([2.0, 2.0, 2.0])
surv_int_ref = distribution.computeProbability(
    ot.Interval([3.0, 3.0, 3.0], [5.0, 5.0, 5.0])
)
ott.assert_almost_equal(surv_int, surv_int_ref)
surv_nonint = distribution.computeSurvivalFunction([2.5, 2.5, 2.5])
ott.assert_almost_equal(surv_nonint, surv_int_ref)

# Test PDF when all zero probabilities
dist_allzero = ot.Multinomial(3, [0.0, 0.0, 0.0])
pdf_allzero = dist_allzero.computePDF([0.0, 0.0, 0.0])
ott.assert_almost_equal(pdf_allzero, 1.0)
pdf_allzero_off = dist_allzero.computePDF([1.0, 0.0, 0.0])
ott.assert_almost_equal(pdf_allzero_off, 0.0)

# Test CDF with slightly negative point
cdf_neg = distribution.computeCDF([-1e-14, 0.0, 0.0])
ott.assert_almost_equal(cdf_neg, 0.0)

# Test computeProbability for degenerate p=[1.0] case
dist_p1 = ot.Multinomial(5, [1.0])
prob_cover_n = dist_p1.computeProbability(ot.Interval([3.0], [6.0]))
ott.assert_almost_equal(prob_cover_n, 1.0)
prob_miss_n = dist_p1.computeProbability(ot.Interval([0.0], [4.0]))
ott.assert_almost_equal(prob_miss_n, 0.0)

# Test isElliptical always false
assert not dist_p1.isElliptical()
assert not ot.Multinomial(5, [0.5, 0.5]).isElliptical()

# Test conditional methods when sumP == 0
dist_cond_zero = ot.Multinomial(5, [0.0, 0.0, 1.0])
ott.assert_almost_equal(dist_cond_zero.computeConditionalPDF(0.0, [0.0]), 1.0)
ott.assert_almost_equal(dist_cond_zero.computeConditionalCDF(0.0, [0.0]), 1.0)
ott.assert_almost_equal(dist_cond_zero.computeConditionalQuantile(0.5, [0.0]), 0.0)

# Conditional methods when sumP == 1.0
dist_cond_sum1 = ot.Multinomial(5, [0.5, 0.5])
ott.assert_almost_equal(dist_cond_sum1.computeConditionalPDF(0.0, [1.0]), 0.0)
ott.assert_almost_equal(dist_cond_sum1.computeConditionalPDF(4.0, [1.0]), 1.0)

# Test sequential conditional PDF consistency
seq_pdf = distribution.computeSequentialConditionalPDF([1.0, 1.0, 1.0])
ott.assert_almost_equal(seq_pdf[0], distribution.computeConditionalPDF(1.0, []))
ott.assert_almost_equal(seq_pdf[1], distribution.computeConditionalPDF(1.0, [1.0]))
ott.assert_almost_equal(seq_pdf[2], distribution.computeConditionalPDF(1.0, [1.0, 1.0]))

# Conditional quantile roundtrip
cond_quant_rt = distribution.computeConditionalQuantile(0.75, [1.0])
cond_cdf_rt = distribution.computeConditionalCDF(cond_quant_rt, [1.0])
assert cond_cdf_rt > 0.75

# CDF corner shortcut
cdf_corner = ot.Multinomial(2, [0.5, 0.5])
ott.assert_almost_equal(cdf_corner.computeCDF([1.1, 1.9]), 0.5)
cdf_corner3 = ot.Multinomial(3, [0.4, 0.6])
ott.assert_almost_equal(cdf_corner3.computeCDF([3.5, 0.0]), 0.064)
cdf_degen = ot.Multinomial(5, [0.0, 1.0])
ott.assert_almost_equal(cdf_degen.computeCDF([0.0, 5.0 + 1e-10]), 1.0)
ott.assert_almost_equal(cdf_degen.computeCDF([0.0, 4.5]), 0.0)

# 1D computeProbability edge cases
prob_1d = ot.Multinomial(1, [0.5])
ott.assert_almost_equal(prob_1d.computeProbability(ot.Interval([0.53], [2.98])), 0.5)
ott.assert_almost_equal(prob_1d.computeProbability(ot.Interval([-1.0], [2.98])), 1.0)
prob_zero = ot.Multinomial(3, [0.0])
ott.assert_almost_equal(prob_zero.computeProbability(ot.Interval([-4.0], [-1e-15])), 0.0)
ott.assert_almost_equal(prob_zero.computeProbability(ot.Interval([-1.0], [1.0])), 1.0)

# computeProbability with a strictly negative upper bound or a lower bound
# above n must return 0.0 in the multivariate case (regression: the negative
# floor used to wrap around the unsigned integer bounds and return garbage)
prob_neg = ot.Multinomial(5, [0.5, 0.5])
ott.assert_almost_equal(
    prob_neg.computeProbability(ot.Interval([-4.0, -4.0], [-1.0, -1.0])), 0.0
)
ott.assert_almost_equal(
    prob_neg.computeProbability(ot.Interval([-1.0, -1.0], [-0.1, -0.1])), 0.0
)
ott.assert_almost_equal(
    prob_neg.computeProbability(ot.Interval([-4.0, 1.0], [-1.0, 2.0])), 0.0
)
ott.assert_almost_equal(
    prob_neg.computeProbability(ot.Interval([6.2, 1.0], [7.0, 2.0])), 0.0
)
# interval with one strictly negative bound and an atom lying on the others
ott.assert_almost_equal(
    prob_neg.computeProbability(ot.Interval([-1.0, 0.0], [-0.1, 2.0])), 0.0
)

# 1D degenerate CDF at p = 1
cdf_p1 = ot.Multinomial(5, [1.0])
ott.assert_almost_equal(cdf_p1.computeCDF([5.0]), 1.0)
ott.assert_almost_equal(cdf_p1.computeCDF([4.0]), 0.0)

# Conditional CDF out of the conditional support
cond_degen = ot.Multinomial(5, [0.3, 0.7])
ott.assert_almost_equal(cond_degen.computeConditionalCDF(-3.0, [1.0]), 0.0)
ott.assert_almost_equal(cond_degen.computeConditionalCDF(-1.0, [1.0]), 0.0)
ott.assert_almost_equal(cond_degen.computeConditionalCDF(5.0, [2.0]), 1.0)
ott.assert_almost_equal(cond_degen.computeConditionalCDF(3.0, [2.0]), 1.0)
ott.assert_almost_equal(cond_degen.computeConditionalCDF(2.0, [2.0]), 0.0)

# Negative points
pdf_neg = ot.Multinomial(2, [0.5, 0.5])
ott.assert_almost_equal(pdf_neg.computePDF([-1.0, 2.0]), 0.0)
ott.assert_almost_equal(pdf_neg.computePDF([0.5, 1.5]), 0.0)
ott.assert_almost_equal(cond_degen.computeConditionalPDF(-2.0, [1.0]), 0.0)

# Exhaustive coverage against brute force


def iter_atoms(n, dimension, defective):
    atoms = []

    def rec(i, rem, acc):
        if i == dimension - 1:
            if defective:
                for v in range(rem + 1):
                    atoms.append(acc + [v])
            else:
                atoms.append(acc + [rem])
        else:
            for v in range(rem + 1):
                rec(i + 1, rem - v, acc + [v])

    rec(0, n, [])
    return atoms


def pdf_formula(n, p, x):
    s = sum(x)
    if s > n:
        return 0.0
    q = sum(p)
    pdf = math.factorial(n)
    for xi in x:
        pdf /= math.factorial(xi)
    pdf /= math.factorial(n - s)
    for i, xi in enumerate(x):
        pdf *= p[i] ** xi
    pdf *= (1.0 - q) ** (n - s)
    return pdf


def check_distribution(n, p, defective):
    dist = ot.Multinomial(n, p)
    atoms = iter_atoms(n, len(p), defective)
    mass = [(a, pdf_formula(n, p, a)) for a in atoms]
    ott.assert_almost_equal(sum(m for _, m in mass), 1.0)
    for atom, ref in mass:
        ott.assert_almost_equal(dist.computePDF(atom), ref, 1e-12, 1e-15)
    for x, ref in mass:
        rec = sum(m for y, m in mass if all(y[i] <= x[i] for i in range(len(p))))
        ott.assert_almost_equal(dist.computeCDF(x), rec, 1e-9, 1e-9)
    for x, ref in mass:
        ott.assert_almost_equal(dist.computeProbability(ot.Interval(x, x)), ref, 1e-9, 1e-9)
        ott.assert_almost_equal(dist.computeProbability(ot.Interval(len(p) * [0.0], x)), dist.computeCDF(x), 1e-9, 1e-9)
    for x, _ in mass:
        rec = sum(m for y, m in mass if all(y[i] > x[i] for i in range(len(p))))
        ott.assert_almost_equal(dist.computeSurvivalFunction(x), rec, 1e-9, 1e-9)
    for a, _ in mass:
        for b, _ in mass:
            if all(a[i] <= b[i] for i in range(len(p))):
                assert dist.computeCDF(a) <= dist.computeCDF(b) + 1e-12
    return dist


check_distribution(5, [0.25, 0.25, 0.25], True)
check_distribution(4, [0.3, 0.4, 0.2], True)
check_distribution(3, [0.4, 0.6], False)
check_distribution(2, [0.5, 0.5], False)
check_distribution(5, [0.0, 1.0], False)

# Conditional methods against the reference binomial
cond_ref = ot.Binomial(3, 0.5)
for x in range(-3, 8):
    ott.assert_almost_equal(distribution.computeConditionalCDF(float(x), [1.0, 1.0]),
                            cond_ref.computeCDF(float(x)), 1e-12, 1e-12)
    ott.assert_almost_equal(distribution.computeConditionalPDF(float(x), [1.0, 1.0]),
                            cond_ref.computePDF(float(x)), 1e-12, 1e-12)
for q in (0.1, 0.5, 0.9):
    ott.assert_almost_equal(distribution.computeConditionalQuantile(q, [1.0, 1.0]),
                            cond_ref.computeQuantile(q)[0], 1e-12, 1e-12)

# Accessors round trips
dist_acc = ot.Multinomial(7, [0.3, 0.4, 0.3])
dist_acc.setN(9)
ott.assert_almost_equal(dist_acc.getN(), 9)
ott.assert_almost_equal(dist_acc.getMean(), [9.0 * 0.3, 9.0 * 0.4, 9.0 * 0.3])
dist_acc.setP([0.5, 0.5, 0.0])
ott.assert_almost_equal(dist_acc.getP(), [0.5, 0.5, 0.0])
dist_acc.setEta(1e-11)
ott.assert_almost_equal(dist_acc.getEta(), 1e-11)
dist_acc.setSmallA(20.0)
ott.assert_almost_equal(dist_acc.getSmallA(), 20.0)
dist_setn = ot.Multinomial(2, [0.5, 0.5])
dist_setn.setN(4)
for x, ref in [(a, pdf_formula(4, [0.5, 0.5], a)) for a in iter_atoms(4, 2, False)]:
    rec = sum(m for y, m in [(a, pdf_formula(4, [0.5, 0.5], a)) for a in iter_atoms(4, 2, False)] if all(y[i] <= x[i] for i in range(2)))
    ott.assert_almost_equal(dist_setn.computeCDF(x), rec, 1e-9, 1e-9)

# Regression: below-mode cap in computeLocalPhi must not silently widen the
# truncated-Poisson window.  The old code exited its descending tail loop
# prematurely when the cap a was below the Poisson mode lambda*|z|, causing
# computeCDF to return wrong values for boxes with an upper bound far below
# the mode of the corresponding Poisson-GF component.
dist_reg = ot.Multinomial(300, [0.3, 0.3, 0.4])
# Empty box: sum of upper bounds = 289 < 300 → CDF must be 0
ott.assert_almost_equal(dist_reg.computeCDF([21.0, 130.0, 138.0]), 0.0)
# Deep-tail CDF: cap 11 well below mode 90; was 1.0 (bug), now ≈ 1.225e-31
ott.assert_almost_equal(
    dist_reg.computeCDF([11.0, 264.0, 287.0]), 1.225e-31, 1e-6, 1e-6
)
# Box probability: [170,190]×full×full was 0.9997 (bug), now ≈ 9.07e-22
ott.assert_almost_equal(
    dist_reg.computeProbability(
        ot.Interval([170.0, 0.0, 0.0], [190.0, 300.0, 300.0])
    ),
    9.07e-22,
    1e-5,
    1e-5,
)
# Medium-size regression (n=40): cap 11 < mode 12 for cat-0, sum>40
dist_reg2 = ot.Multinomial(40, [0.3, 0.3, 0.4])
ott.assert_almost_equal(
    dist_reg2.computeCDF([11.0, 30.0, 30.0]), 0.4406, 1e-4, 1e-4
)

print("OK")

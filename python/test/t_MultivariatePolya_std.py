#! /usr/bin/env python

import math

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

# Instantiate one distribution object
distribution = otexp.MultivariatePolya(5, [1, 1, 1], 1)

assert not distribution.isElliptical()
assert distribution.isContinuous() is False
assert distribution.isIntegral()

# Realization
oneRealization = distribution.getRealization()
assert oneRealization.getDimension() == 3

# Sampling
size = 10000
oneSample = distribution.getSample(size)
assert oneSample.getSize() == size

# Support
support = distribution.getSupport()
assert support.getSize() == 21
interval = ot.Interval([0.0, 0.0, 0.0], [2.0, 2.0, 2.0])
restrictedSupport = distribution.getSupport(interval)
assert restrictedSupport.getSize() > 0

# PDF / CDF at a point on the support
point = ot.Point([1.0, 2.0, 2.0])
LPDF = distribution.computeLogPDF(point)
PDF = distribution.computePDF(point)
CDF = distribution.computeCDF(point)
assert math.isfinite(LPDF)
assert PDF >= 0.0
assert 0.0 <= CDF <= 1.0
# Log-PDF of an off-support point is -inf (zero probability)
assert math.isinf(distribution.computeLogPDF([1.0, 1.0, 1.0]))
survival = distribution.computeSurvivalFunction(point)
assert 0.0 <= survival <= 1.0
quantile = distribution.computeQuantile(0.95)
assert quantile.getDimension() == distribution.getDimension()
assert distribution.computeCDF(quantile) >= 0.95
mean = distribution.getMean()
assert mean.getDimension() == distribution.getDimension()
covariance = distribution.getCovariance()
assert covariance.getDimension() == distribution.getDimension()
parameters = distribution.getParametersCollection()
parameter = distribution.getParameter()
assert parameter.getDimension() > 0
distribution.setParameter(parameter)

marginal1 = distribution.getMarginal(0)
assert marginal1.getDimension() == 1

# Exhaustive coverage against brute force


def log_pochhammer(x, m, c):
    return sum(math.log(x + k * c) for k in range(m))


def pdf_formula(n, alpha, c, x):
    q = sum(alpha)
    log_pdf = math.lgamma(n + 1)
    log_pdf += sum(log_pochhammer(alpha[i], x[i], c) - math.lgamma(x[i] + 1) for i in range(len(alpha)))
    log_pdf -= log_pochhammer(q, n, c)
    return math.exp(log_pdf)


def iter_atoms(n, dim):
    atoms = []

    def rec(i, rem, acc):
        if i == dim - 1:
            atoms.append(acc + [rem])
        else:
            for v in range(rem + 1):
                rec(i + 1, rem - v, acc + [v])

    rec(0, n, [])
    return atoms


def check_distribution(n, alpha, c):
    dist = otexp.MultivariatePolya(n, alpha, c)
    dim = len(alpha)
    atoms = iter_atoms(n, dim)
    mass = [(a, pdf_formula(n, alpha, c, a)) for a in atoms]
    ott.assert_almost_equal(sum(m for _, m in mass), 1.0)
    for atom, ref in mass:
        ott.assert_almost_equal(dist.computePDF(atom), ref, 1e-10, 1e-14)
    for x, ref in mass:
        rec = sum(m for y, m in mass if all(y[i] <= x[i] for i in range(dim)))
        ott.assert_almost_equal(dist.computeCDF(x), rec, 1e-9, 1e-9)
    for x, ref in mass:
        ott.assert_almost_equal(dist.computeProbability(ot.Interval(x, x)), ref, 1e-9, 1e-9)
        ott.assert_almost_equal(
            dist.computeProbability(ot.Interval(dim * [0.0], x)), dist.computeCDF(x), 1e-9, 1e-9
        )
    for x, _ in mass:
        rec_ccdf = sum(m for y, m in mass if not all(y[i] <= x[i] for i in range(dim)))
        ott.assert_almost_equal(dist.computeComplementaryCDF(x), rec_ccdf, 1e-9, 1e-9)
        rec_surv = sum(m for y, m in mass if all(y[i] > x[i] for i in range(dim)))
        ott.assert_almost_equal(dist.computeSurvivalFunction(x), rec_surv, 1e-9, 1e-9)
        rec_surv0 = sum(m for y, m in mass if all(y[i] > x[i] - 0.5 for i in range(dim)))
        ott.assert_almost_equal(dist.computeSurvivalFunction([v - 0.5 for v in x]), rec_surv0, 1e-9, 1e-9)
    for a, _ in mass:
        for b, _ in mass:
            if all(a[i] <= b[i] for i in range(dim)):
                assert dist.computeCDF(a) <= dist.computeCDF(b) + 1e-12
    return dist, atoms, mass


check_distribution(3, [3, 4], 1)
check_distribution(5, [7, 2], 2)
check_distribution(6, [2, 6], 0.5)
check_distribution(3, [3, 3, 3], 1)
check_distribution(4, [2, 3, 4], 0.7)
check_distribution(2, [1, 1, 1, 1], 1)
check_distribution(5, [1, 1, 1], 0)


def betabinomial_pdf(n, a, b, k):
    return (
        math.comb(n, k)
        * (math.gamma(a + k) / math.gamma(a))
        * (math.gamma(b + n - k) / math.gamma(b))
        / (math.gamma(a + b + n) / math.gamma(a + b))
    )


dist3 = otexp.MultivariatePolya(5, [3, 3, 4], 2)
for x in range(0, 6):
    ref = betabinomial_pdf(5, 3 / 2, 7 / 2, x)
    ott.assert_almost_equal(dist3.getMarginal(0).computePDF(x), ref, 1e-12, 1e-12)

m2 = dist3.getMarginal([0, 1])
for x0, x1 in [(0, 2), (1, 1), (2, 2), (3, 0)]:
    ref = pdf_formula(5, [3, 3, 4], 2, [x0, x1, 5 - x0 - x1])
    ott.assert_almost_equal(m2.computePDF([x0, x1]), ref, 1e-12, 1e-12)

# A full marginal with permuted indices must reorder alpha
dist_perm = otexp.MultivariatePolya(5, [1.0, 2.0, 3.0], 1.0)
mean_perm = dist_perm.getMean()
m120 = dist_perm.getMarginal([1, 2, 0])
ott.assert_almost_equal(m120.getMean(), [mean_perm[1], mean_perm[2], mean_perm[0]], 1e-12)
m210 = dist_perm.getMarginal([2, 1, 0])
ott.assert_almost_equal(m210.getMean(), [mean_perm[2], mean_perm[1], mean_perm[0]], 1e-12)
# The identity full marginal is unchanged
ott.assert_almost_equal(dist_perm.getMarginal([0, 1, 2]).getMean(), mean_perm, 1e-12)
# The degenerate c = 0 limit reorders alpha too
polya0perm = otexp.MultivariatePolya(5, [1.0, 2.0, 3.0], 0.0)
mean0 = polya0perm.getMean()
ott.assert_almost_equal(polya0perm.getMarginal([1, 2, 0]).getMean(), [mean0[1], mean0[2], mean0[0]], 1e-12)

q = 10.0
theta = q / 2.0
over = (5 + theta) / (1 + theta)
ott.assert_almost_equal(dist3.getCovariance()[0, 0], 5 * (3 / q) * (1 - 3 / q) * over, 1e-12, 1e-12)
ott.assert_almost_equal(dist3.getCovariance()[0, 1], -5 * (3 / q) * (3 / q) * over, 1e-12, 1e-12)

# Degenerate multinomial limit c = 0
multi = ot.Multinomial(5, [0.25, 0.5, 0.25])
polya0 = otexp.MultivariatePolya(5, [1, 2, 1], 0)
for x in [(0, 5, 0), (1, 2, 2), (2, 3, 0), (4, 1, 0)]:
    ott.assert_almost_equal(polya0.computePDF(x), multi.computePDF(x), 1e-12, 1e-12)
    ott.assert_almost_equal(polya0.computeCDF(x), multi.computeCDF(x), 1e-12, 1e-12)
ott.assert_almost_equal(polya0.getMarginal(0).computePDF(2), multi.getMarginal(0).computePDF(2), 1e-12, 1e-12)
assert polya0.getSupport().getSize() == multi.getSupport().getSize()

# Deterministic 1D limit
det = otexp.MultivariatePolya(4, [5], 2)
ott.assert_almost_equal(det.computePDF([4]), 1.0)
ott.assert_almost_equal(det.computeCDF([4]), 1.0)
ott.assert_almost_equal(det.computeCDF([3]), 0.0)
ott.assert_almost_equal(det.computeProbability(ot.Interval([4], [4])), 1.0)

# Accessors round trips
dist_acc = otexp.MultivariatePolya(5, [3, 3, 3], 1)
ott.assert_almost_equal(dist_acc.getN(), 5)
dist_acc.setN(7)
ott.assert_almost_equal(dist_acc.getN(), 7)
ott.assert_almost_equal(dist_acc.getAlpha(), [3, 3, 3])
dist_acc.setAlpha([2, 4, 4])
ott.assert_almost_equal(dist_acc.getAlpha(), [2, 4, 4])
ott.assert_almost_equal(dist_acc.getC(), 1.0)
dist_acc.setC(0.5)
ott.assert_almost_equal(dist_acc.getC(), 0.5)
dist_acc.setEta(1e-11)
ott.assert_almost_equal(dist_acc.getEta(), 1e-11)
dist_acc.setSmallA(20.0)
ott.assert_almost_equal(dist_acc.getSmallA(), 20.0)
dist_setn = otexp.MultivariatePolya(2, [3, 3], 1)
dist_setn.setN(3)
for x, ref in [(a, pdf_formula(3, [3, 3], 1, a)) for a in iter_atoms(3, 2)]:
    rec = sum(
        m
        for y, m in [(a, pdf_formula(3, [3, 3], 1, a)) for a in iter_atoms(3, 2)]
        if all(y[i] <= x[i] for i in range(2))
    )
    ott.assert_almost_equal(dist_setn.computeCDF(x), rec, 1e-9, 1e-9)

# ---------------- regression: large-n box probabilities (protection) ----------------
# Box probabilities for a large-n MultivariatePolya where the marginal of each
# category has very different dispersion (alpha/c << 1).
dist_reg = otexp.MultivariatePolya(300, [0.3, 0.3, 0.4], 2.0)
ott.assert_almost_equal(
    dist_reg.computeProbability(
        ot.Interval([170.0, 0.0, 0.0], [190.0, 300.0, 300.0])
    ),
    0.02195,
    1e-4,
    1e-4,
)
ott.assert_almost_equal(
    dist_reg.computeProbability(
        ot.Interval([21.0, 0.0, 0.0], [60.0, 300.0, 300.0])
    ),
    0.09581,
    1e-4,
    1e-4,
)

# DistributionValidation
ot.Log.Show(ot.Log.TRACE)
validation = ott.DistributionValidation(distribution)
validation.skipDependenceMeasures()  # Too expensive
validation.skipMinimumVolumeInterval()  # defined for continuous distributions only
validation.skipMinimumVolumeLevelSet()  # defined for continuous distributions only
validation.run()

print("OK")

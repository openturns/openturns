#! /usr/bin/env python

import math

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

# Instantiate one distribution object
distribution = otexp.MultivariateHypergeometric(5, [3, 3, 3])

# Is this distribution elliptical ?
assert not distribution.isElliptical()

# Test for realization of distribution
oneRealization = distribution.getRealization()
assert oneRealization.getDimension() == 3

# Test for sampling
size = 10000
oneSample = distribution.getSample(size)
assert oneSample.getSize() == size
assert oneSample.computeMean().getDimension() == 3
assert oneSample.computeCovariance().getDimension() == 3

interval = ot.Interval([0.0, 0.0, 0.0], [2.0, 2.0, 2.0])
supportInInterval = distribution.getSupport(interval)
assert all(interval.contains(p) for p in supportInInterval)

# Define a point
point = ot.Point(distribution.getDimension(), 1.0)

# Show PDF and CDF at point
LPDF = distribution.computeLogPDF(point)
# (1, 1, 1) is off the support (1+1+1 != 5): zero probability, so log-PDF is -inf
assert math.isinf(LPDF)
PDF = distribution.computePDF(point)
assert PDF == 0.0
CDF = distribution.computeCDF(point)
prob = distribution.computeProbability(
    ot.Interval([0.0, 1.0, 2.0], [0.5, 1.5, 2.5])
)
assert prob >= 0.0
quantile = distribution.computeQuantile(0.95)
assert distribution.computeCDF(quantile) >= 0.95

mean = distribution.getMean()
covariance = distribution.getCovariance()
correlation = distribution.getCorrelation()
assert mean.getDimension() == 3
assert covariance.getDimension() == 3
assert correlation.getDimension() == 3
parameters = distribution.getParametersCollection()
parameter = distribution.getParameter()
assert distribution.getParameterDescription().getSize() == parameter.getDimension()
distribution.setParameter(parameter)

# Support of n=5, colors=[3,3,3]: 12 atoms with x_0+x_1+x_2=5 and x_i <= 3
assert distribution.getSupport().getSize() == 12

# A huge finite bound must be clamped, not wrapped on UnsignedInteger cast
ott.assert_almost_equal(distribution.computeCDF([1e300, 1e300, 1e300]), 1.0)
ott.assert_almost_equal(
    distribution.computeProbability(ot.Interval([1e300, 1e300, 1e300], [1e301, 1e301, 1e301])),
    0.0,
)

marginal1 = distribution.getMarginal(0)
assert marginal1.getDimension() == 1
marginal2 = distribution.getMarginal([0, 1])

# ---------------- exhaustive coverage against brute force ----------------


def iter_atoms(n, colors):
    atoms = []

    def rec(i, rem, acc):
        if i == len(colors) - 1:
            if rem <= colors[i]:
                atoms.append(acc + [rem])
        else:
            for v in range(min(rem, colors[i]) + 1):
                rec(i + 1, rem - v, acc + [v])

    rec(0, n, [])
    return atoms


def pdf_formula(n, colors, x):
    return math.prod(math.comb(colors[i], x[i]) for i in range(len(colors))) / math.comb(
        sum(colors), n
    )


def check_distribution(n, colors):
    dist = otexp.MultivariateHypergeometric(n, colors)
    atoms = iter_atoms(n, colors)
    dim = len(colors)
    mass = [(a, pdf_formula(n, colors, a)) for a in atoms]
    # the PDF sums to one over the support and matches the closed form
    ott.assert_almost_equal(sum(m for _, m in mass), 1.0)
    for atom, ref in mass:
        ott.assert_almost_equal(dist.computePDF(atom), ref, 1e-12, 1e-15)
    # CDF as the partial sum of the PDF over the box [0, x]
    for x, ref in mass:
        rec = sum(m for y, m in mass if all(y[i] <= x[i] for i in range(dim)))
        ott.assert_almost_equal(dist.computeCDF(x), rec, 1e-9, 1e-9)
    # probability of a single atom and of the box [0, x]
    for x, ref in mass:
        ott.assert_almost_equal(dist.computeProbability(ot.Interval(x, x)), ref, 1e-9, 1e-9)
        ott.assert_almost_equal(
            dist.computeProbability(ot.Interval(dim * [0.0], x)), dist.computeCDF(x), 1e-9, 1e-9
        )
    # complementary CDF and survival function as partial sums
    for x, _ in mass:
        rec_ccdf = sum(m for y, m in mass if not all(y[i] <= x[i] for i in range(dim)))
        ott.assert_almost_equal(dist.computeComplementaryCDF(x), rec_ccdf, 1e-9, 1e-9)
        rec_surv = sum(m for y, m in mass if all(y[i] > x[i] for i in range(dim)))
        ott.assert_almost_equal(dist.computeSurvivalFunction(x), rec_surv, 1e-9, 1e-9)
        # survival function with non-integer thresholds and negative bounds
        rec_surv0 = sum(m for y, m in mass if all(y[i] > x[i] - 0.5 for i in range(dim)))
        ott.assert_almost_equal(dist.computeSurvivalFunction([v - 0.5 for v in x]), rec_surv0, 1e-9, 1e-9)
    # CDF is monotone with respect to the component-wise partial order
    for a, _ in mass:
        for b, _ in mass:
            if all(a[i] <= b[i] for i in range(dim)):
                assert dist.computeCDF(a) <= dist.computeCDF(b) + 1e-12
    return dist, atoms, mass


check_distribution(3, [3, 4])
check_distribution(5, [7, 2])
check_distribution(6, [2, 6])
check_distribution(3, [3, 3, 3])
check_distribution(4, [2, 3, 4])
check_distribution(2, [1, 1, 1, 1])

# ---------------- conditional/marginal referent checks ----------------
# The 1D marginal follows the univariate hypergeometric distribution
dist3, atoms3, mass3 = check_distribution(5, [3, 3, 3])
hyper = ot.Hypergeometric(9, 3, 5)
for x in range(-1, 8):
    ott.assert_almost_equal(dist3.getMarginal(0).computePDF(x), hyper.computePDF(x), 1e-12, 1e-12)
    ott.assert_almost_equal(dist3.getMarginal(0).computeCDF(x), hyper.computeCDF(x), 1e-12, 1e-12)
    ott.assert_almost_equal(dist3.getMarginal(0).computeSurvivalFunction(x), hyper.computeSurvivalFunction(x), 1e-12, 1e-12)
# The 2D marginal probabilities match the closed form with a rest category
m2 = dist3.getMarginal([0, 1])
for x0, x1 in [(0, 2), (1, 1), (2, 2), (3, 0)]:
    ref = math.comb(3, x0) * math.comb(3, x1) * math.comb(3, 5 - x0 - x1) / math.comb(9, 5)
    ott.assert_almost_equal(m2.computePDF([x0, x1]), ref, 1e-12, 1e-12)
# The covariance of the 2D marginal
ott.assert_almost_equal(dist3.getCovariance()[0, 0], 5.0 / 9.0, 1e-12, 1e-12)
ott.assert_almost_equal(dist3.getCovariance()[0, 1], -5.0 / 18.0, 1e-12, 1e-12)
ott.assert_almost_equal(dist3.getCorrelation()[0, 1], -0.5, 1e-12, 1e-12)

# The 1D general case matches the univariate hypergeometric distribution
# through the [c, C - c] two-color representation and its marginal
hyper1 = ot.Hypergeometric(8, 3, 4)
dist1 = otexp.MultivariateHypergeometric(4, [3, 5]).getMarginal(0)
for x in range(-1, 7):
    ott.assert_almost_equal(dist1.computePDF(x), hyper1.computePDF(x), 1e-12, 1e-12)
    ott.assert_almost_equal(dist1.computeCDF(x), hyper1.computeCDF(x), 1e-12, 1e-12)
    ott.assert_almost_equal(dist1.computeProbability(ot.Interval([0.5], [2.5])),
                            hyper1.computeProbability(ot.Interval([0.5], [2.5])), 1e-12, 1e-12)

# Deterministic 1D limit: a single color category with the full box
det = otexp.MultivariateHypergeometric(4, [4])
ott.assert_almost_equal(det.computePDF([4]), 1.0)
ott.assert_almost_equal(det.computeCDF([4]), 1.0)
ott.assert_almost_equal(det.computeCDF([3]), 0.0)
ott.assert_almost_equal(det.computeProbability(ot.Interval([4], [4])), 1.0)
ott.assert_almost_equal(det.computeProbability(ot.Interval([0], [3])), 0.0)

# Deterministic 2D limit: n == total draws every ball, the mass sits on (2,2)
det_n = otexp.MultivariateHypergeometric(4, [2, 2])
ott.assert_almost_equal(det_n.computePDF([2, 2]), 1.0)
ott.assert_almost_equal(det_n.computePDF([3, 1]), 0.0)
ott.assert_almost_equal(det_n.computeCDF([1.5, 1.5]), 0.0)
ott.assert_almost_equal(det_n.computeCDF([2, 2]), 1.0)

# ---------------- accessors round trips ----------------
dist_acc = otexp.MultivariateHypergeometric(5, [3, 3, 3])
ott.assert_almost_equal(dist_acc.getN(), 5)
dist_acc.setN(7)
ott.assert_almost_equal(dist_acc.getN(), 7)
ott.assert_almost_equal(dist_acc.getColors(), [3, 3, 3])
dist_acc.setColors([2, 4, 4])
ott.assert_almost_equal(dist_acc.getColors(), [2, 4, 4])
dist_acc.setEta(1e-11)
ott.assert_almost_equal(dist_acc.getEta(), 1e-11)
dist_acc.setSmallA(20.0)
ott.assert_almost_equal(dist_acc.getSmallA(), 20.0)
# The internal Poisson machinery is recomputed after setN/setEta: the CDF must
# remain consistent with the brute force on a small case
dist_setn = otexp.MultivariateHypergeometric(2, [3, 3])
dist_setn.setN(3)
for x, ref in [(a, pdf_formula(3, [3, 3], a)) for a in iter_atoms(3, [3, 3])]:
    rec = sum(
        m
        for y, m in [(a, pdf_formula(3, [3, 3], a)) for a in iter_atoms(3, [3, 3])]
        if all(y[i] <= x[i] for i in range(2))
    )
    ott.assert_almost_equal(dist_setn.computeCDF(x), rec, 1e-9, 1e-9)

# ---------------- regression: below-mode box fix (large n) ----------------
# With n=300, colors=[300,300,400] the mode of the binomial kernel on the first
# category is about 90. Boxes [21,60] and [170,190] lie entirely below/above
# the mode respectively. The old code (no mode check in the 4-arg path)
# wrongly removed non-monotone tails and returned 0.999... for [170,190]×full.
dist_reg = otexp.MultivariateHypergeometric(300, [300, 300, 400])
ott.assert_almost_equal(
    dist_reg.computeProbability(
        ot.Interval([170.0, 0.0, 0.0], [190.0, 300.0, 300.0])
    ),
    4.543e-32,
    1e-6,
    1e-6,
)
ott.assert_almost_equal(
    dist_reg.computeProbability(
        ot.Interval([21.0, 0.0, 0.0], [60.0, 300.0, 300.0])
    ),
    2.780e-6,
    1e-6,
    1e-6,
)

# ---------------- regression: permuted marginal keeps component order ----------------
dist_perm = otexp.MultivariateHypergeometric(5, [3, 4, 2])
m01 = dist_perm.getMarginal([0, 1])
m10 = dist_perm.getMarginal([1, 0])
ott.assert_almost_equal(m01.getMean(), [5.0 / 3.0, 20.0 / 9.0])
ott.assert_almost_equal(m10.getMean(), [20.0 / 9.0, 5.0 / 3.0])
# The full-size shortcut is restricted to the identity order, so a permuted
# full marginal must reorder the components too
m012 = dist_perm.getMarginal([0, 1, 2])
m120 = dist_perm.getMarginal([1, 2, 0])
ott.assert_almost_equal(m012.getMean(), [5.0 / 3.0, 20.0 / 9.0, 10.0 / 9.0])
ott.assert_almost_equal(m120.getMean(), [20.0 / 9.0, 10.0 / 9.0, 5.0 / 3.0])
# The first component of the permuted marginal is the original component 1
ott.assert_almost_equal(dist_perm.getMarginal([1, 0]).getMarginal(0).getMean(),
                        dist_perm.getMarginal(1).getMean(), 1e-12)

# ---------------- regression: setParameter restores eta through the setters ----------------
# setParameter reconstructs the object; a non-default eta must be preserved and
# its spectral state refreshed so computeCDF stays consistent
dist_eta = otexp.MultivariateHypergeometric(5, [3, 3, 3])
dist_eta.setEta(0.5)
dist_eta.setParameter([4, 3, 3, 3])
ott.assert_almost_equal(dist_eta.getEta(), 0.5)
dist_eta_ref = otexp.MultivariateHypergeometric(4, [3, 3, 3])
dist_eta_ref.setEta(0.5)
ott.assert_almost_equal(dist_eta.computeCDF([2, 2, 2]), dist_eta_ref.computeCDF([2, 2, 2]), 1e-12)

# computeLogPDF on-support point matches log(computePDF)
for a, _ in mass3:
    ott.assert_almost_equal(dist3.computeLogPDF(a), math.log(dist3.computePDF(a)), 1e-12, 1e-15)

# DistributionValidation
ot.Log.Show(ot.Log.TRACE)
validation = ott.DistributionValidation(distribution)
validation.skipDependenceMeasures()  # Too expensive
validation.skipMinimumVolumeInterval()  # defined for continuous distributions only
validation.skipMinimumVolumeLevelSet()  # defined for continuous distributions only
validation.run()

print("OK")

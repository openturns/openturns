#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
try:
    import numpy as np
except ModuleNotFoundError:
    np = None
from math import sqrt

ot.TESTPREAMBLE()

# The numerical fixes under test rely on the generic algorithms, so make sure
# that no atom simplification kicks in
ot.ResourceMap.SetAsBool("LinearCombinationDistribution-SimplifyAtoms", False)

# Near-unit weights must be displayed as unit coefficients, both for positive
# and negative weights. The comparison used to be based on the string
# representation of the weight, which is fragile wrt the output precision,
# so raise it in order to reveal the difference.
print("near-unit weights display")
savedPrecision = ot.PlatformInfo.GetNumericalPrecision()
ot.PlatformInfo.SetNumericalPrecision(17)
atom = ot.Normal()
unitStr = "LinearCombinationDistribution(" + atom.__str__() + ")"
negUnitStr = "LinearCombinationDistribution(-" + atom.__str__() + ")"
# ot.SpecFunc.Precision is the threshold used by __str__
nearUnit = 1.0 - 0.5 * ot.SpecFunc.ScalarEpsilon
for weight, expected in [
    (1.0, unitStr),
    (nearUnit, unitStr),
    (-1.0, negUnitStr),
    (-nearUnit, negUnitStr),
]:
    distribution = ot.LinearCombinationDistribution([atom], ot.Point([weight]))
    print(distribution)
    assert str(distribution) == expected, "wrong string for weight=" + repr(weight)
for weight, expected in [
    (2.5, "LinearCombinationDistribution(2.5 * Normal(mu = 0, sigma = 1))"),
    (
        -1.5,
        "LinearCombinationDistribution(-1.5 * Normal(mu = 0, sigma = 1))",
    ),
]:
    distribution = ot.LinearCombinationDistribution([atom], ot.Point([weight]))
    print(distribution)
    assert str(distribution) == expected, "wrong string for weight=" + repr(weight)

# Same check for non-leading terms, with mixed signs
coll = [ot.Normal(), ot.Uniform(-1.0, 1.0)]
for weights, expected in [
    (
        ot.Point([1.0, 1.0]),
        "LinearCombinationDistribution(Normal(mu = 0, sigma = 1) + Uniform(a = -1, b = 1))",
    ),
    (
        ot.Point([1.0, nearUnit]),
        "LinearCombinationDistribution(Normal(mu = 0, sigma = 1) + Uniform(a = -1, b = 1))",
    ),
    (
        ot.Point([1.0, -1.0]),
        "LinearCombinationDistribution(Normal(mu = 0, sigma = 1) - Uniform(a = -1, b = 1))",
    ),
    (
        ot.Point([1.0, -nearUnit]),
        "LinearCombinationDistribution(Normal(mu = 0, sigma = 1) - Uniform(a = -1, b = 1))",
    ),
    (
        ot.Point([1.0, 2.5]),
        "LinearCombinationDistribution(Normal(mu = 0, sigma = 1) + 2.5 * Uniform(a = -1, b = 1))",
    ),
    (
        ot.Point([1.0, -2.5]),
        "LinearCombinationDistribution(Normal(mu = 0, sigma = 1) - 2.5 * Uniform(a = -1, b = 1))",
    ),
]:
    distribution = ot.LinearCombinationDistribution(coll, weights)
    print(distribution)
    assert str(distribution) == expected, "wrong string for weights=" + repr(weights)
ot.PlatformInfo.SetNumericalPrecision(savedPrecision)


# Quantile of a mixture with more than two atoms. The incremental Newton
# iteration used to evaluate the CDF increment on the wrong interval (x was
# updated before the interval was built), yielding inaccurate quantiles.
print("quantiles with more than two atoms")
atoms = [ot.Normal(0.0, 1.0), ot.Normal(1.0, 2.0), ot.Normal(-2.0, 0.5)]
distribution = ot.LinearCombinationDistribution(atoms)
meanRef = sum(atom.getMean()[0] for atom in atoms)
stdDevRef = sqrt(sum(atom.getStandardDeviation()[0] ** 2 for atom in atoms))
reference = ot.Normal(meanRef, stdDevRef)
for prob in [0.001, 0.05, 0.3, 0.5, 0.7, 0.95, 0.999]:
    quantile = distribution.computeQuantile(prob)[0]
    ott.assert_almost_equal(
        distribution.computeCDF(quantile),
        prob,
        1e-9,
        1e-10,
        "cdf(quantile) for prob=%.6g" % prob,
    )
    ott.assert_almost_equal(
        quantile,
        reference.computeQuantile(prob)[0],
        1e-9,
        1e-10,
        "quantile vs reference for prob=%.6g" % prob,
    )
    quantileComp = distribution.computeQuantile(prob, True)[0]
    ott.assert_almost_equal(
        distribution.computeSurvivalFunction(quantileComp),
        prob,
        1e-9,
        1e-10,
        "survival(complementary quantile) for prob=%.6g" % prob,
    )
    ott.assert_almost_equal(
        distribution.computeCDF(quantileComp),
        1.0 - prob,
        1e-9,
        1e-10,
        "cdf(complementary quantile) for prob=%.6g" % prob,
    )
    ott.assert_almost_equal(
        quantileComp,
        reference.computeQuantile(prob, True)[0],
        1e-9,
        1e-10,
        "complementary quantile vs reference for prob=%.6g" % prob,
    )

# Same self-consistency check for a mixture of non-Normal atoms, for which no
# closed-form reference is available
atoms = [ot.Uniform(-1.0, 0.0), ot.Uniform(0.0, 2.0), ot.Exponential(0.5)]
distribution = ot.LinearCombinationDistribution(atoms)
for prob in [0.01, 0.25, 0.5, 0.75, 0.99]:
    quantile = distribution.computeQuantile(prob)[0]
    ott.assert_almost_equal(
        distribution.computeCDF(quantile),
        prob,
        1e-9,
        1e-10,
        "cdf(quantile) for prob=%.6g" % prob,
    )
    quantileComp = distribution.computeQuantile(prob, True)[0]
    ott.assert_almost_equal(
        distribution.computeSurvivalFunction(quantileComp),
        prob,
        1e-9,
        1e-10,
        "survival(complementary quantile) for prob=%.6g" % prob,
    )

# The complementary CDF must match 1 - CDF on both sides of the median, which
# exercises both branches of its implementation
median = distribution.computeQuantile(0.5)[0]
sigma = distribution.getStandardDeviation()[0]
for x in [median - sigma, median, median + sigma]:
    ott.assert_almost_equal(
        distribution.computeComplementaryCDF(x),
        1.0 - distribution.computeCDF(x),
        1e-9,
        1e-10,
        "complementary cdf",
    )


# computeProbability() temporarily tightens pdfPrecision_, which used to leak:
# subsequent PDF evaluations were silently changed by the exceptional
# probability evaluation
print("precision restoration after probability evaluation")

# Raise the base PDF precision so that the temporary tightening performed by
# computeProbability() is large enough to be visible if it leaks
ot.ResourceMap.SetAsScalar("LinearCombinationDistribution-DefaultPDFEpsilon", 1.0e-2)

# Case 1: multivariate mixture, always handled by the generic algorithm
collection = [ot.Normal(), ot.Normal(1.0, 2.0), ot.Normal(-2.0, 1.0)]
weights = ot.Matrix([[1.0, 2.0, -1.0], [0.5, -1.0, 2.0]])
distribution2D = ot.LinearCombinationDistribution(collection, weights)
point = ot.Point([0.3, -0.7])
pdfBefore = distribution2D.computePDF(point)
interval = ot.Interval([-1.0, -2.0], [2.0, 1.0])
probability = distribution2D.computeProbability(interval)
# The probability itself must remain accurate wrt the exact Gaussian result
mu = ot.Point(2)
variance = ot.Point([collection[i].getCovariance()[0, 0] for i in range(3)])
for j in range(2):
    mu[j] = sum(weights[j, i] * collection[i].getMean()[0] for i in range(3))
covariance = ot.CovarianceMatrix(2)
for j in range(2):
    for k in range(j + 1):
        covariance[j, k] = sum(
            weights[j, i] * weights[k, i] * variance[i] for i in range(3)
        )
reference = ot.Normal(mu, covariance)
ott.assert_almost_equal(
    probability, reference.computeProbability(interval), 1e-6, 1e-9, "probability"
)
pdfAfter = distribution2D.computePDF(point)
ott.assert_almost_equal(
    pdfAfter,
    pdfBefore,
    1e-15,
    1e-15,
    "2D pdf changed after probability evaluation",
)

# Case 2: large 1D mixture, handled by the generic algorithm as well
size = ot.ResourceMap.GetAsUnsignedInteger("LinearCombinationDistribution-SmallSize")
bigMixture = ot.LinearCombinationDistribution([ot.Uniform(-0.5, 0.5)] * size)
point = ot.Point(1, 3.0)
pdfBefore = bigMixture.computePDF(point)
probability = bigMixture.computeProbability(ot.Interval(2.0, 3.0))
pdfAfter = bigMixture.computePDF(point)
ott.assert_almost_equal(
    pdfAfter,
    pdfBefore,
    1e-15,
    1e-15,
    "1D pdf changed after probability evaluation",
)
ott.assert_almost_equal(
    probability,
    bigMixture.computeCDF(3.0) - bigMixture.computeCDF(2.0),
    1e-6,
    1e-9,
    "probability vs cdf difference",
)
ot.ResourceMap.SetAsScalar("LinearCombinationDistribution-DefaultPDFEpsilon", 1.0e-10)


# The equivalent normal sums stop on relative thresholds. Check the resulting
# accuracy against closed-form references.
print("relative summation thresholds")


def irwinHallCDF(x):
    """CDF of the sum of three standard uniform variables."""
    if x <= 0.0:
        return 0.0
    if x >= 3.0:
        return 1.0
    value = 0.0
    binomial = [1, 3, 3, 1]
    for k in range(4):
        term = x - k
        if term > 0.0:
            value += (-1.0) ** k * binomial[k] * term**3
    return value / 6.0


mixture = ot.LinearCombinationDistribution([ot.Uniform(0.0, 1.0)] * 3)
for a, b in [(0.0, 0.5), (0.5, 1.0), (1.0, 2.0), (2.0, 2.9), (2.9, 3.0), (0.0, 3.0)]:
    probability = mixture.computeProbability(ot.Interval(a, b))
    ott.assert_almost_equal(
        probability,
        irwinHallCDF(b) - irwinHallCDF(a),
        1e-7,
        1e-10,
        "irwin-hall probability on [%.6g, %.6g]" % (a, b),
    )

# PDF and CDF of a Gaussian mixture, including points far in the tails where
# the equivalent normal sum dominates
atoms = [ot.Normal(0.0, 1.0), ot.Normal(1.0, 2.0), ot.Normal(-2.0, 0.5)]
mixture = ot.LinearCombinationDistribution(atoms)
reference = ot.Normal(meanRef, stdDevRef)
for x in [-8.0, -6.0, -3.0, 0.0, 1.0, 4.0, 8.0]:
    ott.assert_almost_equal(
        mixture.computePDF(x),
        reference.computePDF(x),
        1e-8,
        1e-13,
        "pdf vs reference at x=%.6g" % x,
    )
    ott.assert_almost_equal(
        mixture.computeCDF(x),
        reference.computeCDF(x),
        1e-8,
        1e-13,
        "cdf vs reference at x=%.6g" % x,
    )


# Two continuous atoms are handled by direct convolution
posSum = ot.Uniform(-1.0, 1.0) + ot.Uniform(-1.0, 1.0)
negSum = ot.Uniform(-1.0, 1.0) - ot.Uniform(-1.0, 1.0)
triangular = ot.Triangular(-2.0, 0.0, 2.0)
for mixture in [posSum, negSum]:
    for x in [-1.9, -1.0, -0.5, 0.0, 0.5, 1.0, 1.9]:
        ott.assert_almost_equal(
            mixture.computePDF(x),
            triangular.computePDF(x),
            1e-9,
            1e-13,
            "convolution pdf at x=%.6g" % x,
        )
        ott.assert_almost_equal(
            mixture.computeCDF(x),
            triangular.computeCDF(x),
            1e-9,
            1e-13,
            "convolution cdf at x=%.6g" % x,
        )


# Adaptive confidence band: heavy-tailed atoms widen the band until the
# estimated outside mass drops below BetaAdaptationEpsilon
print("adaptive confidence band")
ot.ResourceMap.SetAsBool("LinearCombinationDistribution-SimplifyAtoms", False)
heavy = ot.LinearCombinationDistribution([ot.LogNormal(0.0, 1.0), ot.Normal()], [1.0, 0.1])
upperBound = heavy.getRange().getUpperBound()[0]
ott.assert_almost_equal(upperBound, 75.2081, 1e-3, 0.0, "adapted upper bound")
if np is not None:
    sample = np.asarray(heavy.getSample(500000))[:, 0]
    exceedance = float((sample > upperBound).mean())
    assert exceedance <= 1e-4, "discarded mass too large: %g" % exceedance
quantile = heavy.computeQuantile(0.999)[0]
assert quantile < upperBound, "quantile clipped at the range bound"
ott.assert_almost_equal(
    heavy.computeSurvivalFunction(quantile), 0.001, 1e-5, 0.0, "survival(quantile)"
)

# Gaussian mixture: the union bound is conservative, so the band may widen;
# whatever the width, extreme quantiles must not be clipped
gaussian = ot.LinearCombinationDistribution([ot.Normal(), ot.Normal(1.0, 2.0), ot.Normal(-2.0, 2.0)])
quantile = gaussian.computeQuantile(0.999)[0]
assert quantile < gaussian.getRange().getUpperBound()[0], "quantile clipped"
ott.assert_almost_equal(
    gaussian.computeSurvivalFunction(quantile), 0.001, 1e-4, 0.0, "survival(quantile)"
)

# Bounded atoms: nothing to adapt either
bounded = ot.LinearCombinationDistribution([ot.Uniform(-1.0, 1.0)] * 2, [1.0, 1.0])
ott.assert_almost_equal(
    bounded.getRange().getUpperBound()[0], 2.0, 0.0, 0.0, "bounded upper bound"
)

# Exception paths
print("exception paths")
# invalid constructor arguments raise InvalidArgumentException, surfaced as
# TypeError by the Python bindings
with ott.assert_raises(TypeError):
    ot.LinearCombinationDistribution([])
with ott.assert_raises(TypeError):
    ot.LinearCombinationDistribution([ot.Normal()], ot.Point([1.0, 2.0]))
with ott.assert_raises(TypeError):
    ot.LinearCombinationDistribution([ot.Normal()], ot.Matrix([[1.0]]), [0.0, 0.0])
with ott.assert_raises(TypeError):
    ot.LinearCombinationDistribution([ot.Normal()], ot.Matrix(1, 2))
# dimension > 3 raises InvalidDimensionException
with ott.assert_raises(RuntimeError):
    ot.LinearCombinationDistribution([ot.Normal()], ot.Matrix(4, 1))
with ott.assert_raises(TypeError):
    ot.LinearCombinationDistribution([ot.Normal([0.0, 0.0], [1.0, 1.0])])

# Wrong interval dimension
distribution2D = ot.LinearCombinationDistribution(
    [ot.Normal(), ot.Normal()], ot.Matrix([[1.0, 0.0], [0.0, 1.0]])
)
with ott.assert_raises(TypeError):
    distribution2D.computeProbability(ot.Interval(0.0, 1.0))

# Quantiles are not implemented for multivariate mixtures
with ott.assert_raises(TypeError):
    distribution2D.computeQuantile(0.1, 0.9, 5)

# Non-continuous mixtures with more than one atom
discreteMixture = ot.LinearCombinationDistribution(
    [ot.Bernoulli(0.5)] * 2, ot.Point([1.0, 1.0])
)
with ott.assert_raises(RuntimeError):
    discreteMixture.computePDF(0.5)
with ott.assert_raises(RuntimeError):
    discreteMixture.computeCDF(0.5)

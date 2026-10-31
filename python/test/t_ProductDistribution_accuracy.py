#! /usr/bin/env python

import math

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# The product of two independent uniform variables has a closed-form CDF.
# Check both tails away from the support endpoints.
distribution = ot.ProductDistribution(ot.Uniform(1.0, 2.0), ot.Uniform(3.0, 4.0))
x = 3.001
expectedCDF = x * math.log(x / 3.0) - x + 3.0
ott.assert_almost_equal(distribution.computeCDF(x), expectedCDF, 1.0e-8, 0.0)
ott.assert_almost_equal(
    distribution.computeComplementaryCDF(x), 1.0 - expectedCDF, 1.0e-12, 0.0
)

x = 3.5
ott.assert_almost_equal(distribution.computePDF(x), math.log(x / 3.0), 1.0e-10, 0.0)

x = 7.999
expectedCCDF = 8.0 - x - x * math.log(8.0 / x)
ott.assert_almost_equal(distribution.computeComplementaryCDF(x), expectedCCDF, 1.0e-7, 0.0)
ott.assert_almost_equal(
    distribution.computeCDF(x), 1.0 - expectedCCDF, 1.0e-12, 0.0
)
ott.assert_almost_equal(
    distribution.computeProbability(ot.Interval(7.999, 8.0)), expectedCCDF, 1.0e-7, 0.0
)

# The same product with one negative factor reaches its lower endpoint through
# the upper tail of the positive product.
signedDistribution = ot.ProductDistribution(
    ot.Uniform(-2.0, -1.0), ot.Uniform(3.0, 4.0)
)
x = -7.999
ott.assert_almost_equal(
    signedDistribution.computeCDF(x), expectedCCDF, 1.0e-7, 0.0
)
ott.assert_almost_equal(
    signedDistribution.computeProbability(ot.Interval(-8.0, x)),
    expectedCCDF,
    1.0e-7,
    0.0,
)

# The direct and complementary integrations must agree on both sides of zero.
mixedDistribution = ot.ProductDistribution(
    ot.Uniform(-1.0, 2.0), ot.Normal(1.0, 2.0)
)
for x in [-2.0, -1.0, -0.5, 0.0, 0.5, 1.0, 2.0]:
    ott.assert_almost_equal(
        mixedDistribution.computeCDF(x)
        + mixedDistribution.computeComplementaryCDF(x),
        1.0,
        1.0e-12,
        0.0,
    )

# A product supported on [0, 1] has a logarithmic CDF near zero.  This is the
# case where direct CDF integration used to miss the tail between nodes.
squareDistribution = ot.ProductDistribution(ot.Uniform(0.0, 1.0), ot.Uniform(0.0, 1.0))
ott.assert_almost_equal(
    squareDistribution.computePDF(0.25), -math.log(0.25), 1.0e-10, 0.0
)
for x in [1.0e-13, 1.0e-12, 1.0e-10]:
    expectedCDF = x * (1.0 - math.log(x))
    ott.assert_almost_equal(squareDistribution.computeCDF(x), expectedCDF, 1.0e-10, 0.0)
    ott.assert_almost_equal(
        squareDistribution.computeProbability(ot.Interval(0.0, x)),
        expectedCDF,
        1.0e-10,
        0.0,
    )

x = 1.0 - 1.0e-6
expectedCCDF = (1.0 - x) + x * math.log(x)
ott.assert_almost_equal(squareDistribution.computeComplementaryCDF(x), expectedCCDF, 1.0e-8, 0.0)
ott.assert_almost_equal(
    squareDistribution.computeProbability(ot.Interval(x, 1.0)), expectedCCDF, 1.0e-8, 0.0
)

# Quantiles must invert the stable probability branch, including both tails.
for probability in [1.0e-12, 1.0e-9, 1.0e-6]:
    quantile = squareDistribution.computeQuantile(probability)[0]
    ott.assert_almost_equal(
        squareDistribution.computeCDF(quantile), probability, 1.0e-6, 0.0
    )
for probability in [1.0 - 1.0e-12, 1.0 - 1.0e-9, 1.0 - 1.0e-6]:
    quantile = squareDistribution.computeQuantile(probability)[0]
    ott.assert_almost_equal(
        squareDistribution.computeComplementaryCDF(quantile),
        1.0 - probability,
        1.0e-8,
        0.0,
    )

for probability in [1.0e-12, 1.0e-9, 1.0e-6]:
    quantile = squareDistribution.computeQuantile(probability, True)[0]
    ott.assert_almost_equal(
        squareDistribution.computeComplementaryCDF(quantile),
        probability,
        1.0e-8,
        0.0,
    )

# The same checks for a product whose lower support bound is positive.
boundedDistribution = ot.ProductDistribution(
    ot.Uniform(1.0, 2.0), ot.Uniform(3.0, 4.0)
)
for probability in [1.0e-12, 1.0e-9, 1.0e-6]:
    quantile = boundedDistribution.computeQuantile(probability)[0]
    ott.assert_almost_equal(
        boundedDistribution.computeCDF(quantile), probability, 1.0e-8, 0.0
    )
for probability in [1.0 - 1.0e-12, 1.0 - 1.0e-9, 1.0 - 1.0e-6]:
    quantile = boundedDistribution.computeQuantile(probability)[0]
    ott.assert_almost_equal(
        boundedDistribution.computeComplementaryCDF(quantile),
        1.0 - probability,
        1.0e-8,
        0.0,
    )
for probability in [1.0e-12, 1.0e-9, 1.0e-6]:
    quantile = boundedDistribution.computeQuantile(probability, True)[0]
    ott.assert_almost_equal(
        boundedDistribution.computeComplementaryCDF(quantile),
        probability,
        1.0e-8,
        0.0,
    )

# A vanishing factor density at zero removes the integrable singularity of the
# product density.  The exact density and CDF are 2 (1 - x) and 2 x - x^2.
betaDistribution = ot.ProductDistribution(
    ot.Uniform(0.0, 1.0), ot.Beta(2.0, 1.0, 0.0, 1.0)
)
assert len(betaDistribution.getSingularities()) == 0
ott.assert_almost_equal(betaDistribution.computePDF(0.0), 2.0, 1.0e-8, 0.0)
x = 0.25
ott.assert_almost_equal(betaDistribution.computePDF(x), 2.0 * (1.0 - x), 1.0e-10, 0.0)
ott.assert_almost_equal(betaDistribution.computeCDF(x), 2.0 * x - x * x, 1.0e-10, 0.0)
x = 1.0e-12
ott.assert_almost_equal(betaDistribution.computeCDF(x), 2.0 * x, 1.0e-6, 0.0)
ott.assert_almost_equal(
    betaDistribution.computeProbability(ot.Interval(0.0, x)), 2.0 * x, 1.0e-6, 0.0
)
x = 0.999999
ott.assert_almost_equal(
    betaDistribution.computeComplementaryCDF(x), (1.0 - x) ** 2, 1.0e-8, 0.0
)

# A factor supported away from zero gives a finite density at zero as well:
# the normal factor is conditioned at zero and the other factor contributes
# E[1 / U] over its support.
awayFromZero = ot.ProductDistribution(ot.Normal(), ot.Uniform(1.0, 2.0))
ott.assert_almost_equal(
    awayFromZero.computePDF(0.0),
    ot.Normal().computePDF(0.0) * math.log(2.0),
    1.0e-10,
    0.0,
)

# A central interval must still be correct after the CDF/CCDF difference.
x = 0.25
ott.assert_almost_equal(
    squareDistribution.computeProbability(ot.Interval(x, 0.5)), 0.25, 1.0e-10, 0.0
)

# Two densities positive at zero produce an integrable zero singularity.
singularDistributions = [
    ot.ProductDistribution(ot.Normal(0.0, 1.0), ot.Normal(0.0, 1.0)),
    ot.ProductDistribution(ot.Exponential(), ot.Exponential()),
    ot.ProductDistribution(ot.Uniform(0.0, 1.0), ot.Uniform(0.0, 1.0)),
]
for singularDistribution in singularDistributions:
    assert len(singularDistribution.getSingularities()) == 1
    ott.assert_almost_equal(
        singularDistribution.getSingularities()[0], 0.0, 1.0e-12, 0.0
    )
    assert math.isinf(singularDistribution.computePDF(0.0))

# A factor whose support does not contain zero cannot create a zero
# singularity, even when the product support crosses zero.
for distribution in [
    ot.ProductDistribution(ot.Uniform(1.0, 2.0), ot.Normal(1.0, 2.0)),
    ot.ProductDistribution(ot.Beta(2.0, 1.0, 0.0, 1.0), ot.Normal(0.0, 1.0)),
]:
    assert len(distribution.getSingularities()) == 0

# The former absolute zero shortcut used Distribution-DefaultQuantileEpsilon:
# changing this tolerance must not change a non-negligible density or CDF.
quantileEpsilon = ot.ResourceMap.GetAsScalar("Distribution-DefaultQuantileEpsilon")
try:
    ot.ResourceMap.SetAsScalar("Distribution-DefaultQuantileEpsilon", 0.1)
    x = 0.25
    ott.assert_almost_equal(betaDistribution.computePDF(x), 1.5, 1.0e-10, 0.0)
    ott.assert_almost_equal(betaDistribution.computeCDF(x), 0.4375, 1.0e-10, 0.0)
finally:
    ot.ResourceMap.SetAsScalar("Distribution-DefaultQuantileEpsilon", quantileEpsilon)

# The characteristic function is the integral of the factor characteristic
# function.  The value at t = 11 is a high-accuracy scipy reference and checks
# the sign and the large-argument branch.
distribution = ot.ProductDistribution(ot.Uniform(1.0, 2.0), ot.Uniform(3.0, 4.0))
ott.assert_almost_equal(
    distribution.computeCharacteristicFunction(1.0e-10).imag,
    5.25e-10,
    1.0e-6,
    0.0,
)
# Just above the small-argument shortcut, the direct factor integration must
# keep the same sign.
for argument in [2.0e-8, -2.0e-8]:
    value = distribution.computeCharacteristicFunction(argument)
    ott.assert_almost_equal(value.real, 1.0, 1.0e-10, 0.0)
    ott.assert_almost_equal(value.imag, 5.25 * argument, 1.0e-6, 0.0)
reference = complex(-0.0003914192910051957, -0.002801502338512093)
value = distribution.computeCharacteristicFunction(11.0)
ott.assert_almost_equal(value.real, reference.real, 1.0e-10, 0.0)
ott.assert_almost_equal(value.imag, reference.imag, 1.0e-10, 0.0)
# A real distribution satisfies phi(-t) = conjugate(phi(t)).
value = distribution.computeCharacteristicFunction(-11.0)
ott.assert_almost_equal(value.real, reference.real, 1.0e-10, 0.0)
ott.assert_almost_equal(value.imag, -reference.imag, 1.0e-10, 0.0)

distribution = ot.ProductDistribution(ot.Normal(0.0, 1.0), ot.Normal(0.0, 1.0))
ott.assert_almost_equal(
    distribution.computeCharacteristicFunction(11.0).real,
    1.0 / math.sqrt(1.0 + 11.0 * 11.0),
    1.0e-12,
    0.0,
)

#! /usr/bin/env python

import math

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

# The ratio of two independent standard normal variables is a Cauchy
# distribution.  Checking the density at zero is a regression test for the
# singularity of the old X * (1 / Y) composition.
normal = ot.Normal()
ratio = otexp.RatioDistribution(normal, normal)
assert ratio.getClassName() == "RatioDistribution"
assert ratio.getDimension() == 1
assert ratio.isContinuous()
assert not ratio.isDiscrete()
assert not ratio.isIntegral()
assert not ratio.isElliptical()
assert not ratio.getRange().getFiniteLowerBound()[0]
assert not ratio.getRange().getFiniteUpperBound()[0]

for z, pdf, cdf in [
    (0.0, 1.0 / math.pi, 0.5),
    (1.0, 1.0 / (2.0 * math.pi), 0.75),
    (-1.0, 1.0 / (2.0 * math.pi), 0.25),
    (2.0, 1.0 / (5.0 * math.pi), 0.5 + math.atan(2.0) / math.pi),
]:
    ott.assert_almost_equal(ratio.computePDF([z]), pdf, 1e-10, 1e-10)
    ott.assert_almost_equal(ratio.computeCDF([z]), cdf, 1e-10, 1e-10)

ott.assert_almost_equal(
    ratio.computeProbability(ot.Interval(0.0, 1.0)), 0.25, 1e-9, 1e-9
)
ott.assert_almost_equal(ratio.computeQuantile(0.25), [-1.0], 1e-9, 1e-9)
ott.assert_almost_equal(ratio.computeQuantile(0.5), [0.0], 1e-9, 1e-9)
ott.assert_almost_equal(ratio.computeQuantile(0.75), [1.0], 1e-9, 1e-9)
ott.assert_almost_equal(ratio.computeQuantile(0.75, True), [-1.0], 1e-9, 1e-9)

# The CDF must remain accurate far in the tails, where the contribution of a
# vanishing denominator is concentrated in an interval of width 1 / |z|.
for z in [100.0, 1000.0, 2048.0, 1.0e5, -100.0, -1000.0, -2048.0, -1.0e5]:
    ott.assert_almost_equal(
        ratio.computeCDF([z]), 0.5 + math.atan(z) / math.pi, 1e-12, 1e-12
    )
for z in [100.0, 2048.0, 1.0e5]:
    ott.assert_almost_equal(
        ratio.computeComplementaryCDF([z]),
        math.atan(1.0 / z) / math.pi,
        1e-12,
        1e-15,
    )

# A one-sided denominator support containing zero exercises the remaining
# tail branch.  The exact CDF follows from integrating the uniform numerator
# CDF against the exponential density.
uniform = ot.Uniform(-1.0, 1.0)
exponential = ot.Exponential()
exponential_ratio = otexp.RatioDistribution(uniform, exponential)
for z in [2048.0, -2048.0]:
    a = 1.0 / abs(z)
    if z > 0.0:
        expected = 0.5 * z * (1.0 - (1.0 + a) * math.exp(-a))
        expected += 0.5 * (1.0 + math.exp(-a))
    else:
        expected = 0.5 * (
            (1.0 - math.exp(-a)) - abs(z) * (1.0 - (1.0 + a) * math.exp(-a))
        )
    cdf = exponential_ratio.computeCDF([z])
    ott.assert_almost_equal(cdf, expected, 1e-10, 1e-10)

# The symmetric numerator makes the distribution unchanged when the
# denominator sign is flipped.
negative_exponential = ot.Exponential() * -1.0
negative_ratio = otexp.RatioDistribution(uniform, negative_exponential)
for z in [2048.0, -2048.0]:
    cdf = negative_ratio.computeCDF([z])
    expected = exponential_ratio.computeCDF([z])
    ott.assert_almost_equal(cdf, expected, 1e-10, 1e-10)


# The normal/normal ratio is a standard Cauchy distribution.  The exact
# quantiles exercise both tail branches and the bracket expansion.  Evaluate
# the tangent in its stable form near the tails.
def cauchy_quantile(p):
    if p < 0.5:
        return -1.0 / math.tan(math.pi * p)
    return 1.0 / math.tan(math.pi * (1.0 - p))


for p in [1.0e-6, 1.0 - 1.0e-6, 1.0 - 1.0e-10, 1.0 - 1.0e-12, 1.0 - 1.0e-14]:
    ott.assert_almost_equal(
        ratio.computeQuantile(p),
        [cauchy_quantile(p)],
        1e-10,
        1e-8,
    )
ott.assert_almost_equal(
    ratio.computeQuantile(1.0e-12, True),
    [1.0 / math.tan(math.pi * 1.0e-12)],
    1e-10,
    1e-8,
)
ott.assert_almost_equal(
    ratio.computeCharacteristicFunction(1.0),
    complex(math.exp(-1.0), 0.0),
    1e-9,
    1e-9,
)

# The division operator must dispatch to RatioDistribution for continuous
# operands and must be accurate at zero.
quotient = ot.Normal() / ot.Normal()
assert quotient.getImplementation().getClassName() == "RatioDistribution"
pdf_zero = quotient.computePDF([0.0])
ott.assert_almost_equal(pdf_zero, 1.0 / math.pi, 1e-10, 1e-10)

# A bounded ratio with an analytic density.
bounded = otexp.RatioDistribution(ot.Uniform(0.0, 1.0), ot.Uniform(1.0, 2.0))
ott.assert_almost_equal(bounded.getRange().getLowerBound(), [0.0])
ott.assert_almost_equal(bounded.getRange().getUpperBound(), [1.0])
ott.assert_almost_equal(bounded.computePDF([0.25]), 1.5, 1e-10, 1e-10)
ott.assert_almost_equal(bounded.computePDF([0.75]), 7.0 / 18.0, 1e-10, 1e-10)
ott.assert_almost_equal(bounded.computeCDF([0.25]), 0.375, 1e-10, 1e-10)
ott.assert_almost_equal(bounded.computeCDF([0.5]), 0.75, 1e-10, 1e-10)
ott.assert_almost_equal(bounded.computeCDF([0.75]), 23.0 / 24.0, 1e-10, 1e-10)
ott.assert_almost_equal(
    bounded.computeProbability(ot.Interval(0.25, 0.75)), 7.0 / 12.0, 1e-9, 1e-9
)
ott.assert_almost_equal(bounded.computeQuantile(0.375), [0.25], 1e-9, 1e-9)
ott.assert_almost_equal(bounded.computeQuantile(0.5), [1.0 / 3.0], 1e-9, 1e-9)
# The same ratio with a negative denominator exercises the negative branch of
# the CDF integral.
negative = otexp.RatioDistribution(ot.Uniform(0.0, 1.0), ot.Uniform(-2.0, -1.0))
ott.assert_almost_equal(negative.getRange().getLowerBound(), [-1.0])
ott.assert_almost_equal(negative.getRange().getUpperBound(), [0.0])
ott.assert_almost_equal(negative.computePDF([-0.5]), 1.5, 1e-10, 1e-10)
ott.assert_almost_equal(negative.computeCDF([-0.5]), 0.25, 1e-10, 1e-10)
# The density is concentrated in a very narrow interval close to the upper
# bound: this checks that the integration domain is restricted to the
# numerator support.
ott.assert_almost_equal(
    bounded.computePDF([0.9999]), 0.00010001500200016724, 1e-12, 1e-12
)

mean = 0.5 * math.log(2.0)
variance = 1.0 / 6.0 - mean * mean
ott.assert_almost_equal(bounded.getMean(), [mean], 1e-10, 1e-10)
ott.assert_almost_equal(bounded.getMoment(1), [mean], 1e-10, 1e-10)
ott.assert_almost_equal(bounded.getMoment(2), [1.0 / 6.0], 1e-10, 1e-10)
ott.assert_almost_equal(bounded.getCovariance()[0, 0], variance, 1e-10, 1e-10)

# A denominator whose support only touches zero at an endpoint may have finite
# inverse moments.  The lognormal density vanishes faster than any power at
# zero, while a gamma density with shape k has a finite inverse moment of order
# n as long as k > n.
logNormalRatio = otexp.RatioDistribution(ot.Normal(), ot.LogNormal())
ott.assert_almost_equal(logNormalRatio.getMean(), [0.0], 1e-10, 1e-10)
ott.assert_almost_equal(logNormalRatio.getMoment(1), [0.0], 1e-10, 1e-10)
ott.assert_almost_equal(
    logNormalRatio.getMoment(2), [math.exp(2.0)], 1e-8, 1e-8
)
ott.assert_almost_equal(
    logNormalRatio.getCovariance()[0, 0], math.exp(2.0), 1e-8, 1e-8
)
gammaRatio = otexp.RatioDistribution(ot.Normal(), ot.Gamma(3.0, 1.0))
ott.assert_almost_equal(gammaRatio.getMean(), [0.0], 1e-10, 1e-10)
ott.assert_almost_equal(gammaRatio.getMoment(1), [0.0], 1e-10, 1e-10)
ott.assert_almost_equal(gammaRatio.getMoment(2), [0.5], 1e-8, 1e-8)
ott.assert_almost_equal(gammaRatio.getCovariance()[0, 0], 0.5, 1e-8, 1e-8)

# The first inverse moment of a gamma(k) denominator is finite for k > 1, but
# the second one diverges for k = 2.
partialGammaRatio = otexp.RatioDistribution(ot.Normal(), ot.Gamma(2.0, 1.0))
ott.assert_almost_equal(partialGammaRatio.getMean(), [0.0], 1e-10, 1e-10)
with ott.assert_raises(RuntimeError):
    partialGammaRatio.getCovariance()

# An exponential denominator vanishes linearly at zero: its inverse moments
# diverge and must be reported as not defined instead of returning a large
# finite quadrature value.
divergentRatio = otexp.RatioDistribution(ot.Normal(), ot.Exponential())
with ott.assert_raises(RuntimeError):
    divergentRatio.getMoment(1)
with ott.assert_raises(RuntimeError):
    divergentRatio.getMean()

ott.assert_almost_equal(bounded.getLeft(), ot.Uniform(0.0, 1.0))
ott.assert_almost_equal(bounded.getRight(), ot.Uniform(1.0, 2.0))
ott.assert_almost_equal(bounded.getParameter(), [0.0, 1.0, 1.0, 2.0])
bounded.setParameter([0.5, 1.0, 2.0, 3.0])
ott.assert_almost_equal(bounded.getLeft(), ot.Uniform(0.5, 1.0))
ott.assert_almost_equal(bounded.getRight(), ot.Uniform(2.0, 3.0))

bounded = otexp.RatioDistribution(ot.Uniform(0.0, 1.0), ot.Uniform(1.0, 2.0))
validation = ott.DistributionValidation(bounded)
validation.skipPDFAtLowerBound()
validation.skipEntropy()
validation.setMomentsSamplingSize(100000)
validation.setDomainSamplingSize(10000)
validation.run()

# The ratio of a standard normal variable by a standard normal variable has
# no finite moments because the denominator support contains zero.
for method in [ratio.getMean, ratio.getCovariance, ratio.getSkewness]:
    with ott.assert_raises(RuntimeError):
        method()
with ott.assert_raises(RuntimeError):
    ratio.getMoment(1)

# Error cases
with ott.assert_raises(TypeError):
    ratio.computePDF([1.0, 2.0])
with ott.assert_raises(TypeError):
    ratio.computeCDF([1.0, 2.0])
with ott.assert_raises(TypeError):
    ratio.setParameter([1.0])
with ott.assert_raises(TypeError):
    otexp.RatioDistribution(ot.Normal(2), ot.Normal())
with ott.assert_raises(TypeError):
    otexp.RatioDistribution(ot.Poisson(), ot.Normal())
with ott.assert_raises(TypeError):
    bounded.setLeft(ot.Poisson())
with ott.assert_raises(TypeError):
    bounded.setRight(ot.Normal(2))

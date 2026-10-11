#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Instantiate one distribution object
x = [[1.0], [2.0], [3.0], [3.0]]
p = [0.3, 0.1, 0.6, 0.6]
distribution = ot.FiniteDiscreteDistribution(x, p)
assert distribution.getClassName() == "FiniteDiscreteDistribution"

# Is this distribution elliptical ?
assert not distribution.isElliptical()

# Is this distribution continuous ?
assert not distribution.isContinuous()

# Has this distribution an independent copula ?
assert distribution.hasIndependentCopula()

# Test for realization of distribution
oneRealization = distribution.getRealization()
ott.assert_almost_equal(oneRealization, [3.0], 0.0, 0.0)

# Test for sampling
size = 10
oneSample = distribution.getSample(size)
ott.assert_almost_equal(
    oneSample,
    [[1.0], [3.0], [3.0], [1.0], [3.0], [3.0], [1.0], [3.0], [3.0], [3.0]],
    0.0,
    0.0,
)

# Define a point
point = ot.Point(distribution.getDimension(), 2.0)

# Show PDF and CDF of a point
pointPDF = distribution.computePDF(point)
pointCDF = distribution.computeCDF(point)
ott.assert_almost_equal(point, [2.0], 0.0, 0.0)
ott.assert_almost_equal(pointPDF, 0.0625, 1e-12, 0.0)
ott.assert_almost_equal(pointCDF, 0.25, 1e-12, 0.0)

# Get 95% quantile
quantile = distribution.computeQuantile(0.95)
ott.assert_almost_equal(quantile, [3.0], 0.0, 0.0)
ott.assert_almost_equal(distribution.computeEntropy(), 0.702919, 1e-6, 0.0)

standard = distribution.getStandardRepresentative()
assert standard.getImplementation().getClassName() == "FiniteDiscreteDistribution"
parameter = distribution.getParameter()
ott.assert_almost_equal(parameter, [1.0, 2.0, 3.0, 0.1875, 0.0625, 0.75], 1e-12, 0.0)
assert list(distribution.getParameterDescription()) == [
    "X^0_0",
    "X^0_1",
    "X^0_2",
    "probabilities_0",
    "probabilities_1",
    "probabilities_2",
]
parameter[-1] = 0.3
distribution.setParameter(parameter)
ott.assert_almost_equal(
    distribution.getParameter(), [1.0, 2.0, 3.0, 0.340909, 0.113636, 0.545455], 1e-5, 0.0
)

# To prevent automatic compaction
ot.ResourceMap.SetAsUnsignedInteger("FiniteDiscreteDistribution-SmallSize", 5)
sample = ot.Sample(40, 3)
for i in range(4):
    for j in range(3):
        sample[i, j] = 10 * (i // 3 + 1) + 0.1 * (j + 1)

multivariateUserDefined = ot.FiniteDiscreteDistribution(sample)

# Has this distribution an independent copula ?
assert not multivariateUserDefined.hasIndependentCopula()

margin0 = multivariateUserDefined.getMarginal(0)
ott.assert_almost_equal(margin0.computePDF([0.0]), 0.9, 1e-12, 0.0)
ott.assert_almost_equal(margin0.computePDF([10.1]), 0.075, 1e-12, 0.0)
ott.assert_almost_equal(margin0.computePDF([20.1]), 0.025, 1e-12, 0.0)
margins20 = multivariateUserDefined.getMarginal([2, 0])
ott.assert_almost_equal(margins20.computePDF([0.0, 0.0]), 0.9, 1e-12, 0.0)
ott.assert_almost_equal(margins20.computePDF([10.3, 10.1]), 0.075, 1e-12, 0.0)
ott.assert_almost_equal(margins20.computePDF([20.3, 20.1]), 0.025, 1e-12, 0.0)

# cdf bug
loi_UD = ot.FiniteDiscreteDistribution(
    [[350], [358], [360], [353], [364], [355], [349], [351]]
)
assert loi_UD.computeCDF([349]) == 0.125, "wrong cdf at min"
assert loi_UD.computeCDF([364]) == 1.0, "wrong cdf at max"

ot.Log.Show(ot.Log.TRACE)
validation = ott.DistributionValidation(distribution)
validation.skipParameters()  # probabilities are renormalized so not independent
validation.run()

# Spearman correlation & Kendall tau of a discrete distribution, see #2845.
# The atoms are tied in the first component and of unequal probability, so both
# coefficients depend on how the ties are resolved. Check them against MC
# sampling via DistributionValidation. The generic implementation, which
# integrates the PDF over the range of the distribution, used to return -1
# here (ie 4 * 0 - 1)
ties = ot.FiniteDiscreteDistribution(
    [[0.0, 0.0], [0.0, 1.0], [0.0, 1.0], [1.0, 0.0], [1.0, 1.0],
     [1.0, 1.0], [2.0, 2.0], [2.0, 2.0], [3.0, 1.0]],
    [0.05, 0.1, 0.15, 0.2, 0.05, 0.1, 0.15, 0.1, 0.1],
)
# probabilities are renormalized so not independent
tiesValidation = ott.DistributionValidation(ties)
tiesValidation.skipParameters()
tiesValidation.skipConditional()
tiesValidation.run()

# A distribution with distinct atom coordinates, as in the bug report
ot.RandomGenerator.SetSeed(0)
sample = ot.Normal(2).getSample(20)
pdf = ot.Normal(2).computePDF(sample).asPoint()
distinct = ot.FiniteDiscreteDistribution(sample, pdf)
distinctValidation = ott.DistributionValidation(distinct)
distinctValidation.skipParameters()
distinctValidation.skipConditional()
distinctValidation.run()

# Comonotonic and countermonotonic atoms
ott.assert_almost_equal(
    ot.FiniteDiscreteDistribution([[0.0, 0.0], [1.0, 1.0], [2.0, 2.0]]).getKendallTau()[0, 1], 1.0
)
ott.assert_almost_equal(
    ot.FiniteDiscreteDistribution([[0.0, 3.0], [1.0, 2.0], [2.0, 1.0]]).getKendallTau()[0, 1], -1.0
)

# A stored support above Distribution-MaximumSupportSizeForRankCorrelation
# still uses the exact mid-rank correlation, see #2845
spearmanLimit = ot.ResourceMap.GetAsUnsignedInteger(
    "Distribution-MaximumSupportSizeForRankCorrelation"
)
ot.ResourceMap.SetAsUnsignedInteger("Distribution-MaximumSupportSizeForRankCorrelation", 2)
ott.assert_almost_equal(
    ot.FiniteDiscreteDistribution(
        [[0.0, 0.0], [1.0, 1.0], [2.0, 2.0]]
    ).getSpearmanCorrelation()[0, 1],
    1.0,
)
ot.ResourceMap.SetAsUnsignedInteger(
    "Distribution-MaximumSupportSizeForRankCorrelation", spearmanLimit
)

# alias
distribution = ot.UserDefined(x, p)

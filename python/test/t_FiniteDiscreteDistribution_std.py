#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Instantiate one distribution object
x = [[1.0], [2.0], [3.0], [3.0]]
p = [0.3, 0.1, 0.6, 0.6]
distribution = ot.FiniteDiscreteDistribution(x, p)
print("Distribution ", repr(distribution))
print("Distribution ", distribution)

# Is this distribution elliptical ?
print("Elliptical = ", distribution.isElliptical())

# Is this distribution continuous ?
print("Continuous = ", distribution.isContinuous())

# Has this distribution an independent copula ?
print("Has independent copula = ", distribution.hasIndependentCopula())

# Test for realization of distribution
oneRealization = distribution.getRealization()
print("oneRealization=", repr(oneRealization))

# Test for sampling
size = 10
oneSample = distribution.getSample(size)
print("oneSample=Ok", repr(oneSample))

# Define a point
point = ot.Point(distribution.getDimension(), 2.0)

# Show PDF and CDF of a point
pointPDF = distribution.computePDF(point)
pointCDF = distribution.computeCDF(point)
print("point= ", repr(point), " pdf= %.12g" % pointPDF, " cdf=", pointCDF)

# Get 95% quantile
quantile = distribution.computeQuantile(0.95)
print("Quantile=", repr(quantile))
print("entropy=%.6f" % distribution.computeEntropy())

print("Standard representative=", distribution.getStandardRepresentative())
print("parameter=", distribution.getParameter())
print("parameterDescription=", distribution.getParameterDescription())
parameter = distribution.getParameter()
parameter[-1] = 0.3
distribution.setParameter(parameter)
print("parameter=", distribution.getParameter())

# To prevent automatic compaction
ot.ResourceMap.SetAsUnsignedInteger("FiniteDiscreteDistribution-SmallSize", 5)
sample = ot.Sample(40, 3)
for i in range(4):
    for j in range(3):
        sample[i, j] = 10 * (i // 3 + 1) + 0.1 * (j + 1)

multivariateUserDefined = ot.FiniteDiscreteDistribution(sample)
print("Multivariate FiniteDiscreteDistribution=", multivariateUserDefined)

# Has this distribution an independent copula ?
print("Has independent copula = ", multivariateUserDefined.hasIndependentCopula())

print("Marginal 0=", multivariateUserDefined.getMarginal(0))
print("Marginal (2, 0)=", multivariateUserDefined.getMarginal([2, 0]))

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

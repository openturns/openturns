#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Instantiate one distribution object
x = [[1.0], [2.0], [3.0], [3.0]]
p = [0.3, 0.1, 0.6, 0.6]
distribution = ot.UserDefined(x, p)
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
ot.ResourceMap.SetAsUnsignedInteger("UserDefined-SmallSize", 5)
sample = ot.Sample(40, 3)
for i in range(4):
    for j in range(3):
        sample[i, j] = 10 * (i // 3 + 1) + 0.1 * (j + 1)

multivariateUserDefined = ot.UserDefined(sample)
print("Multivariate UserDefined=", multivariateUserDefined)

# Has this distribution an independent copula ?
print("Has independent copula = ", multivariateUserDefined.hasIndependentCopula())

print("Marginal 0=", multivariateUserDefined.getMarginal(0))
print("Marginal (2, 0)=", multivariateUserDefined.getMarginal([2, 0]))

# cdf bug
loi_UD = ot.UserDefined([[350], [358], [360], [353], [364], [355], [349], [351]])
assert loi_UD.computeCDF([349]) == 0.125, "wrong cdf at min"
assert loi_UD.computeCDF([364]) == 1.0, "wrong cdf at max"

ot.Log.Show(ot.Log.TRACE)
checker = ott.DistributionChecker(distribution)
checker.skipParameters()  # probabilities are renormalized so not independent
checker.run()

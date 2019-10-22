#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing as ott
ot.TESTPREAMBLE()


# Instanciate one distribution object
distribution = ot.DiscreteCompoundDistribution(
    ot.Bernoulli(0.5), ot.Poisson(20.0))
upper_bound = int(distribution.getRange().getUpperBound()[0])
print("Upper bound : {!r}".format(upper_bound))

# Compare with mathematically equal distribution
poisson_distribution = ot.Poisson(10.0)

for i in range(upper_bound):
    ott.assert_almost_equal(distribution.computePDF(
        [i]), poisson_distribution.computePDF([i]))

print("Distribution ", repr(distribution))
print("Distribution ", distribution)

# Is this distribution elliptical ?
print("Elliptical = ", distribution.isElliptical())

# Is this distribution continuous ?
print("Continuous = ", distribution.isContinuous())

# Test for realization of distribution
oneRealization = distribution.getRealization()
print("oneRealization=", repr(oneRealization))

# Test for sampling
size = 10000
oneSample = distribution.getSample(size)
print("oneSample first=", repr(
    oneSample[0]), " last=", repr(oneSample[size - 1]))
print("mean=", repr(oneSample.computeMean()))
print("covariance=", repr(oneSample.computeCovariance()))

# quantile
quantile = distribution.computeQuantile(0.95)
print("quantile=", repr(quantile))
print("cdf(quantile)=%.6f" % distribution.computeCDF(quantile))
print("entropy=%.6f" % distribution.computeEntropy())
mean = distribution.getMean()
print("mean=", repr(mean))
standardDeviation = distribution.getStandardDeviation()
print("standard deviation=", repr(standardDeviation))
skewness = distribution.getSkewness()
print("skewness=", repr(skewness))
kurtosis = distribution.getKurtosis()
print("kurtosis=", repr(kurtosis))
covariance = distribution.getCovariance()
print("covariance=", repr(covariance))
for i in range(6):
    print("standard moment n=", i, " value=",
          distribution.getStandardMoment(i))
print("Standard representative=", distribution.getStandardRepresentative())
#print("probabilities=", distribution.getProbabilities())

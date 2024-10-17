#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Instantiate one distribution object
distribution = ot.Binomial(15, 0.7)
print("Distribution ", repr(distribution))
print("Distribution ", distribution)

# Is this distribution elliptical ?
print("Elliptical = ", distribution.isElliptical())

# Is this distribution continuous ?
print("Continuous = ", distribution.isContinuous())

# Test for realization of distribution
oneRealization = distribution.getRealization()
print("oneRealization=", repr(oneRealization))

# Define a point
point = ot.Point(distribution.getDimension(), 5.0)
print("Point= ", repr(point))

# PDF value
PDF = distribution.computePDF(point)
print("pdf     =%.6f" % PDF)

# derivative of the PDF with regards the parameters of the distribution
CDF = distribution.computeCDF(point)
print("cdf=%.6f" % CDF)
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
parameters = distribution.getParametersCollection()
print("parameters=", repr(parameters))
print("Standard representative=", distribution.getStandardRepresentative())
# Confidence interval
alpha = 0.05
bounds = distribution.computeBilateralConfidenceInterval(1 - alpha)
print("%.2f%% bilateral confidence interval" % ((1 - alpha) * 100), " =", bounds)

# check survival at upper bound
distribution = ot.Binomial(10, 1.0)
assert distribution.computeCDF(10.0) == 1.0
assert distribution.computeComplementaryCDF(10.0) == 0.0
assert distribution.computeSurvivalFunction(10.0) == 0.0

# negative quantile bug
distribution = ot.Binomial(3, 0.5)
assert distribution.computeScalarQuantile(0.9, True) == 0

# quantile bug
alpha = 0.05
beta = 0.05
for n in range(59, 100):
    d = ot.Binomial(n, alpha)
    k = d.computeQuantile(beta)[0]
    p1 = d.computeCDF(k - 1)
    p2 = d.computeCDF(k)
    ok = p1 < beta <= p2
    print(f"n={n} k={k:.0f} p(k-1)={p1:.4f} p(k)={p2:.4f} ok={ok}")
    assert ok

ot.Log.Show(ot.Log.TRACE)
validation = ott.DistributionValidation(distribution)
validation.run()

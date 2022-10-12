#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# Instantiate one distribution object
dim = 2
copula = ot.FarlieGumbelMorgensternCopula(0.7)
copula.setName("a farlieGumbelMorgenstern copula")
print("Copula ", repr(copula))
print("Copula ", copula)
print("Mean ", copula.getMean())
print("Covariance ", copula.getCovariance())
# Is this copula an elliptical distribution?
print("Elliptical distribution= ", copula.isElliptical())

# Is this copula elliptical ?
print("Elliptical copula= ", copula.hasEllipticalCopula())

# Is this copula independent ?
print("Independent copula= ", copula.hasIndependentCopula())

# Test for realization of copula
oneRealization = copula.getRealization()
print("oneRealization=", oneRealization)

# Test for sampling
size = 10
oneSample = copula.getSample(size)
print("oneSample=", oneSample)

# Test for sampling
size = 10000
anotherSample = copula.getSample(size)
print("anotherSample mean=", anotherSample.computeMean())
print("anotherSample covariance=", anotherSample.computeCovariance())

# Define a point
point = ot.Point(dim, 0.2)

# Show PDF and CDF of zero point
zeroPDF = copula.computePDF(point)
zeroCDF = copula.computeCDF(point)
print("point= ", point, " pdf=", zeroPDF, " cdf= %.12g" % zeroCDF)

# Get 50% quantile
quantile = copula.computeQuantile(0.5)
print("Quantile=", quantile)
print("CDF(quantile)= %.12g" % copula.computeCDF(quantile))
# Get 95% survival function
inverseSurvival = ot.Point(copula.computeInverseSurvivalFunction(0.95))
print("InverseSurvival=", repr(inverseSurvival))
print(
    "Survival(inverseSurvival)=%.6f" % copula.computeSurvivalFunction(inverseSurvival)
)
print("entropy=%.6f" % copula.computeEntropy())

# Confidence regions
interval, threshold = copula.computeMinimumVolumeIntervalWithMarginalProbability(0.95)
print("Minimum volume interval=", interval)
print("threshold=", ot.Point(1, threshold))
levelSet, beta = copula.computeMinimumVolumeLevelSetWithThreshold(0.95)
print("Minimum volume level set=", levelSet)
print("beta=", ot.Point(1, beta))
interval, beta = copula.computeBilateralConfidenceIntervalWithMarginalProbability(0.95)
print("Bilateral confidence interval=", interval)
print("beta=", ot.Point(1, beta))
interval, beta = copula.computeUnilateralConfidenceIntervalWithMarginalProbability(
    0.95, False
)
print("Unilateral confidence interval (lower tail)=", interval)
print("beta=", ot.Point(1, beta))
interval, beta = copula.computeUnilateralConfidenceIntervalWithMarginalProbability(
    0.95, True
)
print("Unilateral confidence interval (upper tail)=", interval)
print("beta=", ot.Point(1, beta))

# Extract the marginals
for i in range(dim):
    margin = copula.getMarginal(i)
    print("margin=", margin)
    print("margin PDF=", margin.computePDF(ot.Point(1, 0.25)))
    print("margin CDF=", margin.computeCDF(ot.Point(1, 0.25)))
    print("margin quantile=", margin.computeQuantile(0.95))
    print("margin realization=", margin.getRealization())

# Extract a 2-D marginal
indices = ot.Indices(2, 0)
indices[0] = 1
indices[1] = 0
print("indices=", indices)
margins = copula.getMarginal(indices)
print("margins=", margins)
print("margins PDF=", margins.computePDF(ot.Point(2, 0.25)))
print("margins CDF= %.12g" % margins.computeCDF(ot.Point(2, 0.25)))
quantile = margins.computeQuantile(0.95)
print("margins quantile=", quantile)
print("margins CDF(quantile)= %.12g" % margins.computeCDF(quantile))
print("margins realization=", margins.getRealization())

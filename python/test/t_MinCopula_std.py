#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

dim = 3

copula = ot.MinCopula(dim)
copula.setName("a min copula")
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

# Define a point
point = ot.Point(dim, 0.2)

# Show PDF and CDF of zero point
zeroPDF = copula.computePDF(point)
zeroCDF = copula.computeCDF(point)
print("point= ", point, "pdf=", zeroPDF, "cdf=", zeroCDF)

# Get 50% quantile
quantile = copula.computeQuantile(0.5)
print("Quantile=", quantile)
print("CDF(quantile)=", copula.computeCDF(quantile))

# Get 5% quantile
quantile = copula.computeQuantile(0.95, True)
print("Quantile=", quantile)

# Get 95% survival function
inverseSurvival = ot.Point(copula.computeInverseSurvivalFunction(0.95))
print("InverseSurvival=", repr(inverseSurvival))
print(
    "Survival(inverseSurvival)=%.6f" % copula.computeSurvivalFunction(inverseSurvival)
)
print("entropy=%.6e" % copula.computeEntropy())
# Extract the marginals
for i in range(dim):
    margin = copula.getMarginal(i)
    print("margin=", margin)
    print("margin PDF=", margin.computePDF(ot.Point(1, 0.25)))
    print("margin CDF=", margin.computeCDF(ot.Point(1, 0.25)))
    print("margin quantile=", margin.computeQuantile(0.95))
    print("margin realization=", margin.getRealization())

# Extract a 2-D marginal
indices = [1, 0]
print("indices=", indices)
margins = copula.getMarginal(indices)
print("margins=", margins)
print("margins PDF=", margins.computePDF(ot.Point(2, 0.25)))
print("margins CDF=", margins.computeCDF(ot.Point(2, 0.25)))
quantile = margins.computeQuantile(0.95)
print("margins quantile=", quantile)
print("margins CDF(quantile)=", margins.computeCDF(quantile))
print("margins realization=", margins.getRealization())

ot.Log.Show(ot.Log.TRACE)
checker = ott.DistributionChecker(copula)
checker.run()

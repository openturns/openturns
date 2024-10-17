#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()


# Instantiate one distribution object
dim = 2
copula = ot.JoeCopula(0.5, 0.5, 0.5)
print("Copula ", repr(copula))
print("Copula ", copula)
print("Mean ", repr(copula.getMean()))
print("Covariance ", repr(copula.getCovariance()))

# Is this copula an elliptical distribution?
print("Elliptical distribution= ", copula.isElliptical())

# Is this copula elliptical ?
print("Elliptical copula= ", copula.hasEllipticalCopula())

# Is this copula independent ?
print("Independent copula= ", copula.hasIndependentCopula())

# Test for realization of distribution
oneRealization = copula.getRealization()
print("oneRealization=", repr(oneRealization))

# Test for sampling
size = 10
oneSample = copula.getSample(size)
print("oneSample=", repr(oneSample))

# Define a point
point = ot.Point(dim, 0.2)

# Show PDF and CDF of point
pointPDF = copula.computePDF(point)
pointCDF = copula.computeCDF(point)
print("Point = ", repr(point), " pdf=%.6f" % pointPDF, " cdf=%.6f" % pointCDF)

# Get 50% quantile
quantile = copula.computeQuantile(0.5)
print("Quantile=", repr(quantile))
print("CDF(quantile)=%.6f" % copula.computeCDF(quantile))

# Get 5% quantile
quantile = copula.computeQuantile(0.95, True)
print("Quantile=", repr(quantile))

# Get 95% survival function
inverseSurvival = ot.Point(copula.computeInverseSurvivalFunction(0.95))
print("InverseSurvival=", repr(inverseSurvival))
print(
    "Survival(inverseSurvival)=%.6f" % copula.computeSurvivalFunction(inverseSurvival)
)
print("entropy=%.6f" % copula.computeEntropy())

# Test conditional methods
y = [0.5]
x = 0.6
p = copula.computeConditionalCDF(x, y)
print("CDF(x|y)=%.5g" % p)
print("Quantile(p|y)=%.5g" % copula.computeConditionalQuantile(p, y))

# Extract the marginals
for i in range(dim):
    margin = copula.getMarginal(i)
    print("margin=", repr(margin))
    print("margin PDF=%.6f" % margin.computePDF([0.25]))
    print("margin CDF=%.6f" % margin.computeCDF([0.25]))
    print("margin quantile=", repr(margin.computeQuantile(0.95)))
    print("margin realization=", repr(margin.getRealization()))

# Extract a 2-D marginal
margins = copula.getMarginal([1, 0])
print("margins=", repr(margins))
print("margins PDF=%.6f" % margins.computePDF([0.25] * 2))
print("margins CDF=%.6f" % margins.computeCDF([0.25] * 2))
quantile = ot.Point(margins.computeQuantile(0.95))
print("margins quantile=", repr(quantile))
print("margins CDF(qantile)=%.6f" % margins.computeCDF(quantile))
print("margins realization=", repr(margins.getRealization()))

ot.Log.Show(ot.Log.TRACE)
validation = ott.DistributionValidation(copula)
validation.skipMoments()
validation.skipCorrelation()
validation.run()

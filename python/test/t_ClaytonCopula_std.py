#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()


# Instantiate one distribution object
dim = 2
copula = ot.ClaytonCopula(2.5)
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
    print("margin=", repr(margin))
    print("margin PDF=%.6f" % margin.computePDF(ot.Point(1, 0.25)))
    print("margin CDF=%.6f" % margin.computeCDF(ot.Point(1, 0.25)))
    print("margin quantile=", repr(margin.computeQuantile(0.95)))
    print("margin realization=", repr(margin.getRealization()))

# Extract a 2-D marginal
indices = [1, 0]
print("indices=", repr(indices))
margins = copula.getMarginal(indices)
print("margins=", repr(margins))
print("margins PDF=%.6f" % margins.computePDF(ot.Point(2, 0.25)))
print("margins CDF=%.6f" % margins.computeCDF(ot.Point(2, 0.25)))
quantile = ot.Point(margins.computeQuantile(0.95))
print("margins quantile=", repr(quantile))
print("margins CDF(qantile)=%.6f" % margins.computeCDF(quantile))
print("margins realization=", repr(margins.getRealization()))

# Additional tests for PDF/CDF in extreme cases
# We focus on the main diagonal as it is the most challenging computation
points = [
    [1.0e-12] * 2,
    [1.0e-7] * 2,
    [0.1] * 2,
    [0.5] * 2,
    [0.1] * 2,
    [0.1] * 2,
    [0.1] * 2,
]

thetas = [1.0e-12, 0.9e-8, 1.1e-8, -0.99, 9.9e1, 1.1e2, 1.0e5]
c_py = list()
C_py = list()
for i in range(len(thetas)):
    x = ot.Point(points[i])
    copula = ot.ClaytonCopula(thetas[i])
    print(copula)
    c_py.append(copula.computePDF(x))
    C_py.append(copula.computeCDF(x))
    print("PDF(", x, ")=%.12e" % c_py[i])
    print("CDF(", x, ")=%.12e" % C_py[i])

print("chi=", copula.computeUpperTailDependenceMatrix())
print("chiL=", copula.computeLowerTailDependenceMatrix())

# Test all scenarios of quantile computation
copula = ot.ClaytonCopula(0.5)
large_p = 0.75
ref = [0.86156] * 2
ott.assert_almost_equal(copula.computeQuantile(1.0 - large_p, True), ref, 1e-5, 0.0)
ott.assert_almost_equal(copula.computeQuantile(large_p, False), ref, 1e-5, 0.0)
small_p = ot.SpecFunc.ScalarEpsilon / 2.0
ott.assert_almost_equal(
    copula.computeQuantile(small_p, True), [1.0 - small_p / 2.0] * 2, 1e-5, 0.0
)
ott.assert_almost_equal(
    copula.computeQuantile(1.0 - small_p, False), [1.0 - small_p / 2.0] * 2, 1e-5, 0.0
)

ot.Log.Show(ot.Log.TRACE)
checker = ott.DistributionChecker(copula)
checker.run()

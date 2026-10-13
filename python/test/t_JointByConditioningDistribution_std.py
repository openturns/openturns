#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

conditionedDistribution = ot.Normal()
conditioningDistribution = ot.JointDistribution(
    [ot.Uniform(0.0, 1.0), ot.Uniform(1.0, 2.0)]
)
distribution = ot.JointByConditioningDistribution(
    conditionedDistribution, conditioningDistribution
)
dim = distribution.getDimension()
print("Distribution ", distribution)
print("Parameters ", distribution.getParametersCollection())
print("Mean ", distribution.getMean())
cov = distribution.getCovariance()
cov_ref = ot.CovarianceMatrix(
    [[0.0833333, 0.0, 0.0833333], [0.0, 0.0833333, 0.0], [0.0833333, 0.0, 2.41667]]
)
ott.assert_almost_equal(cov, cov_ref)
# Is this distribution an elliptical distribution?
print("Elliptical distribution= ", distribution.isElliptical())

# Has this distribution an elliptical copula?
print("Elliptical copula= ", distribution.hasEllipticalCopula())

# Has this distribution an independent copula?
print("Independent copula= ", distribution.hasIndependentCopula())

# Test for realization of distribution
oneRealization = distribution.getRealization()
print("oneRealization=", oneRealization)

# Test for sampling
size = 10
oneSample = distribution.getSample(size)
print("oneSample=", oneSample)

# Test for sampling
size = 10000
anotherSample = distribution.getSample(size)
print("anotherSample mean=", anotherSample.computeMean())
print("anotherSample covariance=", anotherSample.computeCovariance())

# Define a point
point = [0.5, 1.5, 1.0]

# Show PDF and CDF of point point
pointPDF = distribution.computePDF(point)
pointCDF = distribution.computeCDF(point)
print("Point point= ", point, " pdf=%.6f" % pointPDF, " cdf=%.6f" % pointCDF)

# Get 95% quantile
quantile = distribution.computeQuantile(0.95)
print("Quantile=", quantile)
print("CDF(quantile)= %.12g" % distribution.computeCDF(quantile))

x = 2.5
y = [0.5, 1.5]
print("conditional PDF=%.5e" % distribution.computeConditionalPDF(x, y))
condCDF = distribution.computeConditionalCDF(x, y)
print("conditional CDF=%.5e" % condCDF)
q = condCDF
print("conditional quantile=%.5e" % distribution.computeConditionalQuantile(q, y))
pt = [i + 0.5 for i in range(dim)]
print("sequential conditional PDF=", distribution.computeSequentialConditionalPDF(pt))
resCDF = distribution.computeSequentialConditionalCDF(pt)
print("sequential conditional CDF(", pt, ")=", resCDF)
print(
    "sequential conditional quantile(",
    resCDF,
    ")=",
    distribution.computeSequentialConditionalQuantile(resCDF),
)

ot.Log.Show(ot.Log.TRACE)
validation = ott.DistributionValidation(distribution)
validation.skipMoments()  # slow
validation.skipCorrelation()  # slow
validation.skipGradient()  # slow
validation.run()

# --- JointByConditioningDistribution: empty conditioning domain gives 0 ---
link = ot.SymbolicFunction(["y"], ["y", "1.0"])
jbcd = ot.JointByConditioningDistribution(ot.Normal(), ot.Normal(), link)
assert jbcd.computeCDF([-100.0, -100.0]) == 0.0

# --- JointByConditioningDistribution: trailing conditioned state is kept ---
link2d = ot.SymbolicFunction(["y"], ["y", "1.0", "y + 1.0", "2.0"])
conditioned2d = ot.JointDistribution([ot.Normal(), ot.Normal()])
jbcd2d = ot.JointByConditioningDistribution(conditioned2d, ot.Uniform(0.0, 1.0), link2d)
reference2d = ot.JointDistribution(
    [ot.Normal(0.3, 1.0), ot.Normal(1.3, 2.0)], ot.IndependentCopula(2)
)
ott.assert_almost_equal(
    jbcd2d.computeConditionalPDF(1.0, [0.3, 0.5]),
    reference2d.computeConditionalPDF(1.0, [0.5]),
    1e-12,
    0.0,
)

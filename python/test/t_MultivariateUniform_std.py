#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

# Instantiate one distribution object
distribution = otexp.MultivariateUniform([0.0, 1.0], [2.0, 5.0])
print("Distribution ", repr(distribution))
print("Distribution ", distribution)

# Get mean and covariance
print("Mean= ", repr(distribution.getMean()))
print("Covariance= ", repr(distribution.getCovariance()))

# Is this distribution elliptical ?
print("Elliptical = ", distribution.isElliptical())

# Test for realization of distribution
oneRealization = distribution.getRealization()
print("oneRealization=", repr(oneRealization))

# Define a point
point = ot.Point([1.0, 3.0])
print("Point= ", repr(point))

# Show PDF and CDF of point
eps = 1e-5

# PDF value
LPDF = distribution.computeLogPDF(point)
print("log pdf=%.6f" % LPDF)
PDF = distribution.computePDF(point)
print("pdf     =%.6f" % PDF)

# derivative of the PDF with regards the parameters of the distribution
CDF = distribution.computeCDF(point)
print("cdf=%.6f" % CDF)
CCDF = distribution.computeComplementaryCDF(point)
print("ccdf=%.6f" % CCDF)

# quantile
quantile = distribution.computeQuantile(0.95)
print("quantile=", repr(quantile))
print("cdf(quantile)=%.6f" % distribution.computeCDF(quantile))
# Get 95% survival function
inverseSurvival = ot.Point(distribution.computeInverseSurvivalFunction(0.95))
print("InverseSurvival=", repr(inverseSurvival))
print(
    "Survival(inverseSurvival)=%.6f"
    % distribution.computeSurvivalFunction(inverseSurvival)
)
print("entropy=%.6f" % distribution.computeEntropy())
# Confidence regions
interval, threshold = distribution.computeMinimumVolumeIntervalWithMarginalProbability(
    0.95
)
print("Minimum volume interval=", interval)
print("threshold=", threshold)
levelSet, beta = distribution.computeMinimumVolumeLevelSetWithThreshold(0.95)
print("Minimum volume level set=", levelSet)
print("beta=", beta)
interval, beta = distribution.computeBilateralConfidenceIntervalWithMarginalProbability(
    0.95
)
print("Bilateral confidence interval=", interval)
print("beta=", beta)
(
    interval,
    beta,
) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, False)
print("Unilateral confidence interval (lower tail)=", interval)
print("beta=", beta)
(
    interval,
    beta,
) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, True)
print("Unilateral confidence interval (upper tail)=", interval)
print("beta=", beta)
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

# Specific tests

# 1D distribution
dist1d = otexp.MultivariateUniform([-1.0], [2.0])
assert dist1d.getDimension() == 1
assert dist1d.isElliptical()
print("1D scalar quantile=%.6f" % dist1d.computeScalarQuantile(0.5))
print("1D scalar quantile (tail)=%.6f" % dist1d.computeScalarQuantile(0.1, True))

# isCopula
assert not distribution.isCopula()
dist_copula = otexp.MultivariateUniform([0.0, 0.0], [1.0, 1.0])
assert dist_copula.isCopula()
print("isCopula OK")

# hasIndependentCopula
assert distribution.hasIndependentCopula()
print("hasIndependentCopula OK")

# Marginal
marg0 = distribution.getMarginal(0)
assert marg0.getDimension() == 1
ott.assert_almost_equal(marg0.getRange().getLowerBound()[0], 0.0)
ott.assert_almost_equal(marg0.getRange().getUpperBound()[0], 2.0)
print("Marginal OK")

marg01 = distribution.getMarginal([0, 1])
assert marg01.getDimension() == 2
print("Marginal 2D OK")

# DDF
ddf = distribution.computeDDF(point)
assert ddf == [0.0, 0.0]
print("DDF OK")

# setA/setB
dist = otexp.MultivariateUniform([0.0, 0.0], [1.0, 1.0])
dist.setA([-1.0, -1.0])
assert dist.getA() == [-1.0, -1.0]
dist.setB([2.0, 2.0])
assert dist.getB() == [2.0, 2.0]
print("setA/setB OK")

# getSample
sample_big = distribution.getSample(100)
assert sample_big.getSize() == 100
print("getSample OK")

# Parameter accessor
param = distribution.getParameter()
assert param[0] == 0.0
assert param[1] == 1.0
assert param[2] == 2.0
assert param[3] == 5.0

dist2 = otexp.MultivariateUniform([0.0, 0.0], [1.0, 1.0])
dist2.setParameter(param)
assert dist2.getA() == [0.0, 1.0]
assert dist2.getB() == [2.0, 5.0]
print("Parameter accessors OK")

# Test invalid constructors
with ott.assert_raises(TypeError):
    otexp.MultivariateUniform([0.0], [1.0, 2.0])
print("Dimension mismatch raises exception: OK")

with ott.assert_raises(TypeError):
    otexp.MultivariateUniform([1.0], [0.0])
print("a >= b raises exception: OK")

# Run DistributionValidation
ot.Log.Show(ot.Log.TRACE)
validation = ott.DistributionValidation(distribution)
validation.run()

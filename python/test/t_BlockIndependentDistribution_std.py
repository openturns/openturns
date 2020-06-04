#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

ot.RandomGenerator.SetSeed(0)

# First test: comparison with a Normal distribution with block-diagonal
# correlation
R0 = ot.CorrelationMatrix(2)
R0[0, 1] = 0.5
R1 = ot.CorrelationMatrix(3)
R1[0, 1] = 0.2
R1[0, 2] = 0.1
R1[1, 2] = 0.15
R2 = ot.CorrelationMatrix(2)
R2[0, 1] = 0.3
collection = [ot.Normal([0.0]*2, [1.0]*2, R0), \
              ot.Normal([0.0]*3, [1.0]*3, R1), \
              ot.Normal([0.0]*2, [1.0]*2, R2)]
distribution = ot.BlockIndependentDistribution(collection)
copulaCollection = [ot.NormalCopula(R0), \
                    ot.NormalCopula(R1), \
                    ot.NormalCopula(R2)]
copula = ot.ComposedCopula(copulaCollection)
ref = ot.ComposedDistribution([ot.Normal(0.0, 1.0)]*7, copula)

# Define a point
point = [0.3]*distribution.getDimension()
print("Point= ", point)

# Show PDF and CDF of point
DDF = distribution.computeDDF( point )
print("ddf      =", DDF)
print("ddf (ref)=", ref.computeDDF(point))
PDF = distribution.computePDF( point )
print("pdf      =%.5f" % PDF)
print("pdf (ref)=%.5f" % ref.computePDF(point))
CDF = distribution.computeCDF( point )
print("cdf      =%.5f" % CDF)
print("cdf (ref)=%.5f" % ref.computeCDF(point))
Survival = distribution.computeSurvivalFunction(point)
print("Survival      =%.5f" % Survival)
print("Survival (ref)=%.5f" % ref.computeSurvivalFunction(point))
InverseSurvival = distribution.computeInverseSurvivalFunction(0.95)
print("Inverse survival      =", InverseSurvival)
print("Inverse survival (ref)=", ref.computeInverseSurvivalFunction(0.95))
print("Survival(inverse survival)=%.5f" % distribution.computeSurvivalFunction(InverseSurvival))
# Get 50% quantile
quantile = distribution.computeQuantile( 0.5 )
print("Quantile      =", quantile)
print("Quantile (ref)=", ref.computeQuantile( 0.5 ))
print("CDF(quantile) =%.5f" % distribution.computeCDF(quantile))

# Instanciate one distribution object
R = ot.CorrelationMatrix(3)
R[0, 1] = 0.5
R[0, 2] = 0.25
collection = [ot.ComposedDistribution([ot.Normal()]*2, ot.AliMikhailHaqCopula(0.5)), \
              ot.Normal([1.0]*3, [2.0]*3, R), \
              ot.ComposedDistribution([ot.Exponential()]*2, ot.FrankCopula(0.5))]
distribution = ot.BlockIndependentDistribution(collection)
print("Distribution ", distribution)

# Is this distribution elliptical ?
print("Elliptical distribution= ", distribution.isElliptical())

# Is this distribution continuous ?
print("Continuous = ", distribution.isContinuous())

# Has this distribution an elliptical copula ?
print("Elliptical = ", distribution.hasEllipticalCopula())

# Has this distribution an independent copula ?
print("Independent = ", distribution.hasIndependentCopula())

# Test for realization of distribution
oneRealization = distribution.getRealization()
print("oneRealization=", oneRealization)

# Test for sampling
size = 10000
oneSample = distribution.getSample( size )
print("oneSample first=", oneSample[0], " last=", oneSample[size - 1])
print("mean=", oneSample.computeMean())
precision = ot.PlatformInfo.GetNumericalPrecision()
ot.PlatformInfo.SetNumericalPrecision(4)
print("covariance=", oneSample.computeCovariance())
ot.PlatformInfo.SetNumericalPrecision(precision)

# Define a point
point = [0.3]*distribution.getDimension()
print("Point= ", point)

# Show PDF and CDF of point
DDF = distribution.computeDDF( point )
print("ddf     =", DDF)
PDF = distribution.computePDF( point )
print("pdf     =%.5f" % PDF)
CDF = distribution.computeCDF( point )
print("cdf=%.5f" % CDF)
Survival = distribution.computeSurvivalFunction(point)
print("Survival      =%.5f" % Survival)
print("Survival (ref)=%.5f" % distribution.computeSurvivalFunction(point))
InverseSurvival = distribution.computeInverseSurvivalFunction(0.95)
print("Inverse survival=", InverseSurvival)
print("Survival(inverse survival)=%.5f" % distribution.computeSurvivalFunction(InverseSurvival))
# Get 50% quantile
quantile = distribution.computeQuantile( 0.5 )
print("Quantile=", quantile)
print("CDF(quantile)=%.5f" % distribution.computeCDF(quantile))

if distribution.getDimension() <= 2:
    # Confidence regions
    interval, threshold = distribution.computeMinimumVolumeIntervalWithMarginalProbability(0.95)
    print("Minimum volume interval=", interval)
    print("threshold=%.5f" % threshold)
    levelSet, beta = distribution.computeMinimumVolumeLevelSetWithThreshold(0.95)
    print("Minimum volume level set=", levelSet)
    print("beta=%.5f" % beta)
    interval, beta = distribution.computeBilateralConfidenceIntervalWithMarginalProbability(0.95)
    print("Bilateral confidence interval=", interval)
    print("beta=%.5f" % beta)
    interval, beta = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, False)
    print("Unilateral confidence interval (lower tail)=", interval)
    print("beta=%.5f" % beta)
    interval, beta = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, True)
    print("Unilateral confidence interval (upper tail)=", interval)
    print("beta=%.5f" % beta)

print("entropy     =%.5f" % distribution.computeEntropy())
print("entropy (MC)=%.5f" % -distribution.computeLogPDF(distribution.getSample(1000000)).computeMean()[0])
mean = distribution.getMean()
# Ensure mean is [0,0,1,1,1,1,1]
# Following platform, the value slightly differs
ott.assert_almost_equal(distribution.getMean(), [0,0,1,1,1,1,1])
ot.PlatformInfo.SetNumericalPrecision(4)
covariance = distribution.getCovariance()
print("covariance=", covariance)
correlation = distribution.getCorrelation()
print("correlation=", correlation)
spearman = distribution.getSpearmanCorrelation()
print("spearman=", spearman)
kendall = distribution.getKendallTau()
print("kendall=", kendall)
ot.PlatformInfo.SetNumericalPrecision(precision)
dim = distribution.getDimension()
x = 0.6
y = [0.2]*(dim - 1)
print("conditional PDF=%.5f" % distribution.computeConditionalPDF(x, y))
print("conditional CDF=%.5f" % distribution.computeConditionalCDF(x, y))
print("conditional quantile=%.5f" % distribution.computeConditionalQuantile(x, y))
pt = ot.Point(dim)
for i in range(dim):
    pt[i] = 0.1 * i + 0.05
print("sequential conditional PDF=", distribution.computeSequentialConditionalPDF(pt))
resCDF = distribution.computeSequentialConditionalCDF(pt)
print("sequential conditional CDF(", pt, ")=", resCDF)
print("sequential conditional quantile(", resCDF, ")=", distribution.computeSequentialConditionalQuantile(resCDF))

# Extract a 5-D marginal
dim = 5
point = [0.25]*dim
indices = [1, 2, 3, 5, 6]
print("indices=", indices)
margins = distribution.getMarginal(indices)
print("margins=", margins)
print("margins PDF=%.5f" % margins.computePDF(point))
print("margins CDF=%.5f" % margins.computeCDF(point))
quantile = margins.computeQuantile(0.95)
print("margins quantile=", quantile)
print("margins CDF(quantile)=%.5f" % margins.computeCDF(quantile))
print("margins realization=", margins.getRealization())
# Tests o the isoprobabilistic transformation
# General case with normal standard distribution
print("isoprobabilistic transformation (general normal)=", distribution.getIsoProbabilisticTransformation())
# General case with non-normal standard distribution
collection[0] = ot.SklarCopula(ot.Student(3.0, [1.0]*2, [3.0]*2, ot.CorrelationMatrix(2)))
distribution = ot.BlockIndependentDistribution(collection)
print("isoprobabilistic transformation (general non-normal)=", distribution.getIsoProbabilisticTransformation())
dim = distribution.getDimension()
x = 0.6
y = [0.2] * (dim - 1)
print("conditional PDF=%.5f" % distribution.computeConditionalPDF(x, y))
print("conditional CDF=%.5f" % distribution.computeConditionalCDF(x, y))
print("conditional quantile=%.5f" % distribution.computeConditionalQuantile(x, y))
pt = ot.Point(dim)
for i in range(dim):
    pt[i] = 0.1 * i + 0.05
print("sequential conditional PDF=", distribution.computeSequentialConditionalPDF(pt))
resCDF = distribution.computeSequentialConditionalCDF(pt)
print("sequential conditional CDF(", pt, ")=", resCDF)
print("sequential conditional quantile(", resCDF, ")=", distribution.computeSequentialConditionalQuantile(resCDF))

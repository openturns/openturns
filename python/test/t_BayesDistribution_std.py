#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

conditionedDistribution = Normal()
conditioningDistribution = ComposedDistribution([Uniform(0.0, 1.0),
                                                 Uniform(1.0, 2.0)])
distribution = BayesDistribution(
    conditionedDistribution, conditioningDistribution)
dim = distribution.getDimension()
print("Distribution ", distribution)
print("Parameters ", distribution.getParametersCollection())
print("Mean ", distribution.getMean())
print("Covariance ", SquareMatrix(distribution.getCovariance()).clean(1.0e-6))
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
point = [1.0, 0.5, 1.5]

# Show PDF and CDF of point point
pointPDF = distribution.computePDF(point)
pointCDF = distribution.computeCDF(point)
print("Point point= ", point, " pdf=%.6f" %
      pointPDF, " cdf=%.6f" % pointCDF)

# Get 95% quantile
quantile = distribution.computeQuantile(0.95)
print("Quantile=", quantile)
print("CDF(quantile)= %.12g" % distribution.computeCDF(quantile))
# The following computations are ok but take too much time
# Get 95% survival function
#   result should be
#   InverseSurvival= class=Point name=Unnamed dimension=3 values=[-2.88594,0.0169382,1.01694]
#   Survival(inverseSurvival)=0.950000
# inverseSurvival = Point(distribution.computeInverseSurvivalFunction(0.95))
# print("InverseSurvival=", repr(inverseSurvival))
# print("Survival(inverseSurvival)=%.6f" %
#       distribution.computeSurvivalFunction(inverseSurvival))

# Confidence regions
#   result should be
#   Minimum volume interval= [-3.37188, 4.37188]
#   [0.00851414, 0.991486]
#   [1.00851, 1.99149]
#   threshold= [0.982972]
# interval, threshold = distribution.computeMinimumVolumeIntervalWithMarginalProbability(0.95)
# print("Minimum volume interval=", interval)
# print("threshold=", Point(1, threshold))
#  result should be
#  Minimum volume level set= {x | f(x) <= 3.24752} with f=
#  MinimumVolumeLevelSetEvaluation(BayesDistribution(X, Y with X|Theta~Normal(Theta), Theta=f(Y), f=IdentityEvaluation(dimension=2), Y~ComposedDistribution(Uniform(a = 0, b = 1), Uniform(a = 1, b = 2), IndependentCopula(dimension = 2))))
#  beta= [0.0388705]
# levelSet, beta = distribution.computeMinimumVolumeLevelSetWithThreshold(0.95)
# print("Minimum volume level set=", levelSet)
# print("beta=", Point(1, beta))
#   result should be
#   Bilateral confidence interval= [-3.37188, 4.37188]
#   [0.00851414, 0.991486]
#   [1.00851, 1.99149]
#   beta= [0.982972]
# interval, beta = distribution.computeBilateralConfidenceIntervalWithMarginalProbability(0.95)
# print("Bilateral confidence interval=", interval)
# print("beta=", Point(1, beta))
#   result should be
#   Unilateral confidence interval (lower tail)= [-15.3011, 3.87457]
#   [0, 0.982796]
#   [1, 1.9828]
#   beta= [0.982796]
# interval, beta = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(
#     0.95, False)
# print("Unilateral confidence interval (lower tail)=", interval)
# print("beta=", Point(1, beta))
#   result should be
#   Unilateral confidence interval (upper tail)= [-2.88594, 16.3011]
#   [0.0169382, 1]
#   [1.01694, 2]
#   beta= [0.983062]
# interval, beta = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(
#     0.95, True)
# print("Unilateral confidence interval (upper tail)=", interval)
# print("beta=", Point(1, beta))

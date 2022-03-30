#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing as ott
import math as m

ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(0)


# Definition of the marginals
X1 = ot.Uniform(-m.pi, m.pi)
X2 = ot.Uniform(-m.pi, m.pi)
X3 = ot.Uniform(-m.pi, m.pi)

# 3d distribution made with independent marginals
distX = ot.ComposedDistribution([X1, X2, X3])

# Get a sample of it
size = 100
X = distX.getSample(size)


# The Ishigami model
modelIshigami = ot.SymbolicFunction(
    ["X1", "X2", "X3"], ["sin(X1) + 5.0 * (sin(X2))^2 + 0.1 * X3^4 * sin(X1)"])

# Apply model: Y = m(X)
Y = modelIshigami(X)

# We define the covariance models for the HSIC indices.
# For the input, we consider a SquaredExponential covariance model.
covarianceList = ot.CovarianceModelCollection()

# Input sample
for i in range(3):
    Xi = X.getMarginal(i)
    Cov = ot.SquaredExponential(1)
    Cov.setScale(Xi.computeStandardDeviation())
    covarianceList.add(Cov)

# Output sample with squared exponential covariance
Cov2 = ot.SquaredExponential(1)
Cov2.setScale(Y.computeStandardDeviation())
covarianceList.add(Cov2)

# We choose an estimator type :
#  - unbiased: HSICUStat;
#   - biased: HSICVStat.
#
estimatorType = ot.HSICUStat()


# We define a distance function for the weights
#  For the TSA, the critical domain is [5,+inf].
interval = ot.Interval(5, float('inf'))
g = ot.DistanceToDomainFunction(interval)

stdDev = Y.computeStandardDeviation()[0]
foo = ot.SymbolicFunction(["x", "s"], ["exp(-x/s)"])
g2 = ot.ParametricFunction(foo, [1], [0.1*stdDev])

# The filter function
filterFunction = ot.ComposedFunction(g2, g)


# We eventually build the HSIC object!
TSA = ot.HSICEstimatorTargetSensitivity(
    covarianceList, X, Y, estimatorType, filterFunction)

# We get the R2-HSIC
R2HSIC = TSA.getR2HSICIndices()
ott.assert_almost_equal(R2HSIC, [0.26863688, 0.00468423, 0.00339962])

# and the HSIC indices
HSICIndices = TSA.getHSICIndices()
ott.assert_almost_equal(HSICIndices, [0.00107494, 0.00001868, 0.00001411])

# We get the asymptotic pvalue
pvaluesAs = TSA.getPValuesAsymptotic()
ott.assert_almost_equal(pvaluesAs, [0.00000000, 0.26201467, 0.28227083])

# We set the number of permutations for the pvalue estimate
b = 1000
TSA.setPermutationSize(b)

# We get the pvalue estimate by permutations
pvaluesPerm = TSA.getPValuesPermutation()
ott.assert_almost_equal(pvaluesPerm, [0.00000000, 0.25674326, 0.23776224])

# Change the filter function and recompute everything
squaredExponential = ot.SymbolicFunction("x", "exp(-0.1 * x^2)")
alternateFilter = ot.ComposedFunction(squaredExponential, g)
TSA.setFilterFunction(alternateFilter)
ott.assert_almost_equal(TSA.getR2HSICIndices(), [0.263026,0.0041902,0.00309598])
ott.assert_almost_equal(TSA.getHSICIndices(), [1.54349e-05,2.45066e-07,1.88477e-07], 1e-4, 0.0)
ott.assert_almost_equal(TSA.getPValuesPermutation(), [0.0,0.250749,0.271728])
ott.assert_almost_equal(TSA.getPValuesAsymptotic(), [0.0,0.270278,0.288026])

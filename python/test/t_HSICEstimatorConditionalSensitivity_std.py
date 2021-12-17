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
#  - unbiased: HSICUStat (not available here!!);
#  - biased: HSICVStat.
#
estimatorType = ot.HSICVStat()

# We define a distance function for the weights
# For the TSA, the critical domain is [5,+inf].
interval = ot.Interval(5, float('inf'))
g = ot.DistanceToDomainFunction(interval)

stdDev = Y.computeStandardDeviation()[0]
foo = ot.SymbolicFunction(["x", "s"], ["exp(-x/s)"])
g2 = ot.ParametricFunction(foo, [1], [0.1*stdDev])

# The weight function
weight = ot.ComposedFunction(g2, g)

# We eventually build the HSIC object
CSA = ot.HSICEstimatorConditionalSensitivity(
    covarianceList, X, Y, estimatorType, weight)

# We get the R2-HSIC
R2HSIC = CSA.getR2HSICIndices()
ott.assert_almost_equal(R2HSIC, [0.03717355, 0.00524130, 0.23551919])


# and the HSIC indices
HSICIndices = CSA.getHSICIndices()
ott.assert_almost_equal(HSICIndices, [0.00064033, 0.00025769, 0.01105157])

# We set the number of permutations for the pvalue estimate
b = 100
CSA.setPermutationSize(b)

# We get the pvalue estimate by permutations
pvaluesPerm = CSA.getPValuesPermutation()
ott.assert_almost_equal(pvaluesPerm, [0.74257426, 0.95049505, 0.00000000])

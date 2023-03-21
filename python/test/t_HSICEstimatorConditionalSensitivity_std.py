#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
from openturns.usecases import ishigami_function

ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(0)


# Ishigami use-case
ishigami = ishigami_function.IshigamiModel()
distX = ishigami.distributionX

# Get a sample of it
size = 100
X = distX.getSample(size)


# The Ishigami model
modelIshigami = ishigami.model
modelIshigami.setParameter([5, 0.1])

# Apply model: Y = m(X)
Y = modelIshigami(X)

# We define the covariance models for the HSIC indices.
# For the input, we consider a SquaredExponential covariance model.
covarianceModelCollection = ot.CovarianceModelCollection()

# Input sample
for i in range(3):
    Xi = X.getMarginal(i)
    Cov = ot.SquaredExponential(1)
    Cov.setScale(Xi.computeStandardDeviation())
    covarianceModelCollection.add(Cov)

# Output sample with squared exponential covariance
Cov2 = ot.SquaredExponential(1)
Cov2.setScale(Y.computeStandardDeviation())
covarianceModelCollection.add(Cov2)


# We define a distance function for the weights
# For the TSA, the critical domain is [5,+inf].
interval = ot.Interval(5, float("inf"))
g = ot.DistanceToDomainFunction(interval)

stdDev = Y.computeStandardDeviation()[0]
foo = ot.SymbolicFunction(["x", "s"], ["exp(-x/s)"])
g2 = ot.ParametricFunction(foo, [1], [0.1 * stdDev])

# The weight function
weight = ot.ComposedFunction(g2, g)

# We eventually build the HSIC object
# HSICVStat event is already embedded as it is the only one available
# for that kind of analysis
CSA = ot.HSICEstimatorConditionalSensitivity(covarianceModelCollection, X, Y, weight)

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
ott.assert_almost_equal(pvaluesPerm, [0.74257426, 0.94059406, 0.00000000])

# Change the weight function and recompute everything
squaredExponential = ot.SymbolicFunction("x", "exp(-x^2)")
alternateWeight = ot.ComposedFunction(squaredExponential, g)
CSA.setWeightFunction(alternateWeight)
ott.assert_almost_equal(CSA.getR2HSICIndices(), [0.0910527, 0.00738055, 0.166624])
ott.assert_almost_equal(CSA.getHSICIndices(), [0.00218376, 0.000419288, 0.00898721])
ott.assert_almost_equal(CSA.getPValuesPermutation(), [0.287129, 0.881188, 0.00000000])

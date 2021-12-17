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

#  We choose an estimator type :
#   - unbiased: HSICUStat;
#   - biased: HSICVStat.
#
estimatorType = ot.HSICUStat()

# We eventually build the HSIC object!
hsic = ot.HSICEstimatorGlobalSensitivity(covarianceList, X, Y, estimatorType)

# We get the HSIC indices
HSICIndices = hsic.getHSICIndices()
ott.assert_almost_equal(HSICIndices, [0.02228377, 0.00025668, 0.00599247])

# and the R2-HSIC
R2HSIC = hsic.getR2HSICIndices()
ott.assert_almost_equal(R2HSIC, [0.29807297, 0.00344498, 0.07726572])

# We set the bootstrap size for the pvalue estimate
b = 1000
hsic.setPermutationSize(b)

# We get the pvalue estimate by permutations
pvaluesPerm = hsic.getPValuesPermutation()
ott.assert_almost_equal(pvaluesPerm, [0.00000000, 0.30669331, 0.00000000])

pvaluesAs = hsic.getPValuesAsymptotic()
ott.assert_almost_equal(pvaluesAs, [0.00000000, 0.33271992, 0.00165620])


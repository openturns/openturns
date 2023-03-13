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

#  We choose an estimator type :
#   - unbiased: HSICUStat;
#   - biased: HSICVStat.
#
estimatorType = ot.HSICUStat()

# We eventually build the HSIC object!
hsic = ot.HSICEstimatorGlobalSensitivity(covarianceModelCollection, X, Y, estimatorType)

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
ott.assert_almost_equal(pvaluesPerm, [0.00000000, 0.29670330, 0.00199800])

pvaluesAs = hsic.getPValuesAsymptotic()
ott.assert_almost_equal(pvaluesAs, [0.00000000, 0.33271992, 0.00165620])

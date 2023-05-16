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

# Output
Y = modelIshigami(X)

# Using the same covariance model for each marginal
Cov1 = ot.SquaredExponential(1)

# Output covariance model
Cov2 = ot.SquaredExponential(1)

# Set output covariance scale
Cov2.setScale(Y.computeStandardDeviation())

# This is the GSA-type estimator: weight is 1.
W = ot.Point(size, 1.0)

# Using a biased estimator
estimatorTypeV = ot.HSICVStat()

# Loop over marginals
hsicIndexRef = [0.02331323, 0.00205350, 0.00791711]
for i in range(3):
    test = X.getMarginal(i)
    # Set input covariance scale
    Cov1.setScale(test.computeStandardDeviation())
    hsicIndex = estimatorTypeV.computeHSICIndex(
        Cov1.discretize(test), Cov2.discretize(Y), W
    )
    ott.assert_almost_equal(hsicIndex, hsicIndexRef[i])

# Using an unbiased estimator
estimatorTypeU = ot.HSICUStat()

# Loop over marginals
hsicIndexRef = [0.02228377, 0.00025668, 0.00599247]
for i in range(3):
    test = X.getMarginal(i)
    # Set input covariance scale
    Cov1.setScale(test.computeStandardDeviation())
    hsicIndex = estimatorTypeU.computeHSICIndex(
        Cov1.discretize(test), Cov2.discretize(Y), W
    )
    ott.assert_almost_equal(hsicIndex, hsicIndexRef[i])

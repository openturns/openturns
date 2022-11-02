#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import math as m

ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(0)

# All marginals of the Ishigami model
X1 = ot.Uniform(-m.pi, m.pi)
X2 = ot.Uniform(-m.pi, m.pi)
X3 = ot.Uniform(-m.pi, m.pi)
distX = ot.ComposedDistribution([X1, X2, X3])

# Input sample of size 100 and dimension 3
size = 100
X = distX.getSample(size)

# The Ishigami model
modelIshigami = ot.SymbolicFunction(
    ["X1", "X2", "X3"], ["sin(X1) + 5.0 * (sin(X2))^2 + 0.1 * X3^4 * sin(X1)"]
)

# Output
Y = modelIshigami(X)

# Using the same covariance model for each marginal
Cov1 = ot.SquaredExponential(1)

# Output covariance model
Cov2 = ot.SquaredExponential(1)

# Set output covariance scale
Cov2.setScale(Y.computeStandardDeviation())

# This is the GSA-type estimator: weight is 1.
W = ot.SquareMatrix(size)
for i in range(size):
    W[i, i] = 1.0


# Using a biased estimator
estimatorTypeV = ot.HSICVStat()

# Loop over marginals
hsicIndexRef = [0.02331323, 0.00205350, 0.00791711]
for i in range(3):
    test = X.getMarginal(i)
    # Set input covariance scale
    Cov1.setScale(test.computeStandardDeviation())
    hsicIndex = estimatorTypeV.computeHSICIndex(test, Y, Cov1, Cov2, W)
    ott.assert_almost_equal(hsicIndex, hsicIndexRef[i])

# Using an unbiased estimator
estimatorTypeU = ot.HSICUStat()

# Loop over marginals
hsicIndexRef = [0.02228377, 0.00025668, 0.00599247]
for i in range(3):
    test = X.getMarginal(i)
    # Set input covariance scale
    Cov1.setScale(test.computeStandardDeviation())
    hsicIndex = estimatorTypeU.computeHSICIndex(test, Y, Cov1, Cov2, W)
    ott.assert_almost_equal(hsicIndex, hsicIndexRef[i])

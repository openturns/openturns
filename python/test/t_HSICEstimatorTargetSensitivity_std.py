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

# We choose an estimator type :
#  - unbiased: HSICUStat;
#   - biased: HSICVStat.
#
estimatorType = ot.HSICUStat()


# We define a distance function for the weights
#  For the TSA, the critical domain is [5,+inf].
interval = ot.Interval(5, float("inf"))
g = ot.DistanceToDomainFunction(interval)

stdDev = Y.computeStandardDeviation()[0]
foo = ot.SymbolicFunction(["x", "s"], ["exp(-x/s)"])
g2 = ot.ParametricFunction(foo, [1], [0.1 * stdDev])

# The filter function
filterFunction = ot.ComposedFunction(g2, g)

# random generator state
# use the same state for parallel/sequential validation
state = ot.RandomGenerator.GetState()

for key in [True, False]:
    ot.ResourceMap.SetAsBool("HSICEstimator-ParallelPValues", key)
    ot.RandomGenerator.SetState(state)

    # We eventually build the HSIC object!
    TSA = ot.HSICEstimatorTargetSensitivity(
        covarianceModelCollection, X, Y, estimatorType, filterFunction
    )

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
    ott.assert_almost_equal(pvaluesPerm, [0.00000000, 0.23376623, 0.26573427])

    # Change the filter function and recompute everything
    squaredExponential = ot.SymbolicFunction("x", "exp(-0.1 * x^2)")
    alternateFilter = ot.ComposedFunction(squaredExponential, g)
    TSA.setFilterFunction(alternateFilter)
    ott.assert_almost_equal(TSA.getR2HSICIndices(), [0.373511, 0.0130156, 0.0153977])
    ott.assert_almost_equal(
        TSA.getHSICIndices(), [0.00118685, 4.12193e-05, 5.07577e-05], 1e-4, 0.0
    )
    ott.assert_almost_equal(TSA.getPValuesPermutation(), [0, 0.137862, 0.112887])
    ott.assert_almost_equal(TSA.getPValuesAsymptotic(), [7.32022e-13, 0.143851, 0.128866])

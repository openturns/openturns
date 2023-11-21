#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

dimension = 2
sampleSize = 100000

# we create an analytical function
analytical = ot.SymbolicFunction(["x0", "x1"], ["10+3*x0+x1"])

# we create a collection of centered and reduced gaussian distributions
aCollection = [ot.Normal()] * dimension

# we create one distribution object
aDistribution = ot.ComposedDistribution(aCollection)

# Random vectors
randomVector = ot.RandomVector(aDistribution)

# we create two input samples for the function
inputSample = randomVector.getSample(sampleSize)
outputSample = analytical(inputSample)

# Create the CorrelationAnalysis object
corr_analysis = ot.CorrelationAnalysis(inputSample, outputSample)

# Because here outputSample = analyical(inputSample),
# the theoretical squared SRC indices are equal to the Sobol indices
# See theory/reliability_sensitivity/sensitivity_sobol.rst for the computation.
squared_src = corr_analysis.computeSquaredSRC()
ott.assert_almost_equal(squared_src, [0.9, 0.1], 0.0, 1e-2)  # theoretical value

# Squared SRC with normalize
squared_src_normalize = corr_analysis.computeSquaredSRC(True)
ott.assert_almost_equal(
    squared_src_normalize, [0.9, 0.1], 0.0, 1e-2
)  # theoretical value

src = corr_analysis.computeSRC()
ott.assert_almost_equal(
    src, [0.9486832980505138, 0.31622776601683794], 0.0, 1e-2
)  # sqrt of squared_src

srrc = corr_analysis.computeSRRC()
ott.assert_almost_equal(srrc, [0.94, 0.30], 0.0, 1e-2)  # approximate value

pcc = corr_analysis.computePCC()
ott.assert_almost_equal(pcc, [1.0, 1.0], 1e-5, 0.0)  # theoretical value

prcc = corr_analysis.computePRCC()
ott.assert_almost_equal(prcc, [0.99, 0.92], 0.0, 1e-2)  # approximate value

pearson = corr_analysis.computeLinearCorrelation()
ott.assert_almost_equal(pearson, [0.95, 0.31], 0.0, 1e-2)  # approximate value

spearman = corr_analysis.computeSpearmanCorrelation()
ott.assert_almost_equal(spearman, [0.94, 0.30], 0.0, 1e-2)  # approximate value

kendalltau = corr_analysis.computeKendallTau()
ott.assert_almost_equal(kendalltau, [0.79, 0.20], 0.0, 1e-2)  # approximate value

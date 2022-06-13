#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

dimension = 2
sampleSize = 1000

# we create an analytical function
analytical = ot.SymbolicFunction(['x0', 'x1'], ['10+3*x0+x1'])

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

squared_src = corr_analysis.computeSquaredSRC()
ott.assert_almost_equal(squared_src, [0.901238,0.105409], 1e-5, 0.0)

# Squared SRC with normalize
squared_src_normalize = corr_analysis.computeSquaredSRC(True)
ott.assert_almost_equal(squared_src_normalize, [0.895287,0.104713], 1e-5, 0.0)

src = corr_analysis.computeSRC()
ott.assert_almost_equal(src, [0.949336,0.324667], 1e-5, 0.0)

srrc = corr_analysis.computeSRRC()
ott.assert_almost_equal(srrc, [0.946444,0.304684], 1e-5, 0.0)

pcc = corr_analysis.computePCC()
ott.assert_almost_equal(pcc, [1.0, 1.0], 1e-5, 0.0)

prcc = corr_analysis.computePRCC()
ott.assert_almost_equal(prcc, [0.98879,0.905344], 1e-5, 0.0)

pearson = corr_analysis.computePearsonCorrelation()
ott.assert_almost_equal(pearson, [0.945835,0.314431], 1e-5, 0.0)

spearman = corr_analysis.computeSpearmanCorrelation()
ott.assert_almost_equal(spearman, [0.941684,0.289898], 1e-5, 0.0)

kendalltau = corr_analysis.computeKendallTau()
ott.assert_almost_equal(kendalltau, [0.7935375375375376, 0.1965765765765766], 1e-16, 0.0)

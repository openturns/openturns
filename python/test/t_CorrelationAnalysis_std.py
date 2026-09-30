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
aDistribution = ot.JointDistribution(aCollection)

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

# Draw the SRC indices with a dedicated y-axis label, see issue #1363
input_names = analytical.getInputDescription()
graph = ot.SobolIndicesAlgorithm.DrawCorrelationCoefficients(
    src, input_names, "SRC indices", "SRC index"
)
assert graph.getYTitle() == "SRC index", "custom y label"
# default y label is unchanged
graph = ot.SobolIndicesAlgorithm.DrawCorrelationCoefficients(
    src, input_names, "SRC indices"
)
assert graph.getYTitle() == "correlation coefficient", "default y label"

# same for the PointWithDescription overload
pointWithDescription = ot.PointWithDescription(src)
pointWithDescription.setDescription(input_names)
graph = ot.SobolIndicesAlgorithm.DrawCorrelationCoefficients(
    pointWithDescription, "SRC indices", "SRC index"
)
assert graph.getYTitle() == "SRC index", "custom y label (PWD)"

# Check collinearity indices on correlated inputs
beta1 = 2.5
beta2 = 0.3
sigma1 = 1.6
sigma2 = 0.8
sigmaEps = 0.1
r = 0.5

b1 = beta1 * sigma1
b2 = beta2 * sigma2
a = b1 * b1 * (1 - r * r)
c = b2 * b2 * (1 - r * r)
b = (b1 * b1 * r * r) + (2 * b1 * b2 * r) + (b2 * b2 * r * r)
lmg1 = (a + b / 2) / (a + b + c + sigmaEps * sigmaEps)
lmg2 = (c + b / 2) / (a + b + c + sigmaEps * sigmaEps)
pmvd1 = a * (1 + b / (a + c)) / (a + b + c + sigmaEps * sigmaEps)
pmvd2 = c * (1 + b / (a + c)) / (a + b + c + sigmaEps * sigmaEps)
vif12 = 1 / (1 - r * r)

correlatedSampleSize = 100000
ot.RandomGenerator.SetSeed(0)
corMatrix = ot.CorrelationMatrix(2, [1.0, r, r, 1.0])
inputDistribution = ot.Normal([0.0, 0.0], [sigma1, sigma2], corMatrix)
correlatedInputSample = inputDistribution.getSample(correlatedSampleSize)
linearFunction = ot.LinearFunction([0.0, 0.0], [0.0], ot.Matrix(1, 2, [beta1, beta2]))
noiseDistribution = ot.Normal(0.0, sigmaEps)
noiseSample = noiseDistribution.getSample(correlatedSampleSize)
correlatedOutputSample = linearFunction(correlatedInputSample) + noiseSample

analysis = ot.CorrelationAnalysis(correlatedInputSample, correlatedOutputSample)
lmg_computed, pmvd_computed = analysis.computeLMGAndPMVD()
lmg_estimated, pmvd_estimated = analysis.computeLMGAndPMVDMonteCarlo(1000)
ott.assert_almost_equal(lmg_computed, [lmg1, lmg2], 2e-3, 0.0)
ott.assert_almost_equal(lmg_estimated, lmg_computed, 6e-3, 0.0)
ott.assert_almost_equal(pmvd_computed, [pmvd1, pmvd2], 2e-3, 0.0)
ott.assert_almost_equal(pmvd_estimated, pmvd_computed, 4e-3, 0.0)

johnson_computed = analysis.computeJohnson()
ott.assert_almost_equal(johnson_computed, lmg_computed, 1e-10, 0.0)

vif_computed = ot.CorrelationAnalysis.ComputeVIF(correlatedInputSample)
ott.assert_almost_equal(vif_computed, [vif12, vif12], 7e-4, 0.0)

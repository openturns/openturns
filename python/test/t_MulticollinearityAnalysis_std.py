#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Define the model: Y = beta1 * X1 + beta2 * X2 + Noise
beta1 = 2.5
beta2 = 0.3
sigma1 = 1.6
sigma2 = 0.8
sigmaEps = 0.1
r = 0.5

# Compute the theoretical values of the indices
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

# Draw an input sample
sampleSize = 100000
corMatrix = ot.CorrelationMatrix(2, [1.0, r, r, 1.0])
inputDistribution = ot.Normal([0.0, 0.0], [sigma1, sigma2], corMatrix)
inputSample = inputDistribution.getSample(sampleSize)

# Build the output sample
linearFunction = ot.LinearFunction([0.0, 0.0], [0.0], ot.Matrix(1, 2, [beta1, beta2]))
noiseDistribution = ot.Normal(0.0, sigmaEps)
noiseSample = noiseDistribution.getSample(sampleSize)
outputSample = linearFunction(inputSample) + noiseSample

analysis = ot.MulticollinearityAnalysis(inputSample, outputSample)

# LMG and PMVD indices
lmg_computed, pmvd_computed = analysis.computeLmgPmvd()
lmg_estimated, pmvd_estimated = analysis.estimateLmgPmvdMonteCarlo(1000)
print(f"Theoretical LMG = [{lmg1}, {lmg2}]")
print(f"Computed LMG = [{lmg_computed[0]}, {lmg_computed[1]}]")
ott.assert_almost_equal(lmg_computed, [lmg1, lmg2], 2e-3, 0.0)
print(f"Estimated LMG = [{lmg_estimated[0]}, {lmg_estimated[1]}]")
ott.assert_almost_equal(lmg_estimated, lmg_computed, 6e-3, 0.0)
print(f"Theoretical PMVD = [{pmvd1}, {pmvd2}]")
print(f"Computed PMVD = [{pmvd_computed[0]}, {pmvd_computed[1]}]")
ott.assert_almost_equal(pmvd_computed, [pmvd1, pmvd2], 2e-3, 0.0)
print(f"Estimated PMVD = [{pmvd_estimated[0]}, {pmvd_estimated[1]}]")
ott.assert_almost_equal(pmvd_estimated, pmvd_computed, 4e-3, 0.0)

# Johnson index
johnson_computed = analysis.computeJohnson()
print(f"Computed Johnson = [{johnson_computed[0]}, {johnson_computed[1]}]")
ott.assert_almost_equal(johnson_computed, lmg_computed, 1e-12, 0.0)  # In 2D, Johnson and LMG indices are identical

# Check that an exception is raised when outputSample is not provided
analysis = ot.MulticollinearityAnalysis(inputSample)
with ott.assert_raises(TypeError):
    analysis.computeLmgPmvd()
with ott.assert_raises(TypeError):
    analysis.computeJohnson()

# VIF metric
analysis = ot.MulticollinearityAnalysis(inputSample)
vif_computed = analysis.computeVif()
print(f"Theoretical VIF = [{vif12}, {vif12}]")
print(f"Computed VIF = [{vif_computed[0]}, {vif_computed[1]}]")
ott.assert_almost_equal(vif_computed, [vif12, vif12], 7e-4, 0.0)

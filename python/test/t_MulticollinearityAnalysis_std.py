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
sampleSize = 10000
corrMatrix = ot.CorrelationMatrix(2, [1.0, r, r, 1.0])
inputDistribution = ot.Normal([0.0, 0.0], [sigma1, sigma2], corrMatrix)
inputSample = inputDistribution.getSample(sampleSize)

# Build the output sample
linearFunction = ot.LinearFunction([0.0, 0.0], [0.0], ot.Matrix(1, 2, [beta1, beta2]))
noiseDistribution = ot.Normal(0.0, sigmaEps)
noiseSample = noiseDistribution.getSample(sampleSize)
outputSample = linearFunction(inputSample) + noiseSample

analysis = ot.MulticollinearityAnalysis(inputSample, outputSample)

# LMG and PMVD indices
lmg, pmvd = analysis.computeLMGPMVD()
print(f"Theoretical LMG = [{lmg1}, {lmg2}]")
print(f"Computed LMG = [{lmg[0]}, {lmg[1]}]")
ott.assert_almost_equal(lmg, [lmg1, lmg2], 1e-2, 0.0)
print(f"Theoretical PMVD = [{pmvd1}, {pmvd2}]")
print(f"Computed PMVD = [{pmvd[0]}, {pmvd[1]}]")
ott.assert_almost_equal(pmvd, [pmvd1, pmvd2], 1e-2, 0.0)

# LMG and PMVD indices (Monte Carlo estimation)
lmgMC, pmvdMC = analysis.estimateLMGPMVDMonteCarlo(1000)
print(f"Estimated LMG = [{lmgMC[0]}, {lmgMC[1]}]")
ott.assert_almost_equal(lmgMC, [0.84, 0.15], 1e-10, 0.0)
print(f"Estimated PMVD = [{pmvdMC[0]}, {pmvdMC[1]}]")
ott.assert_almost_equal(pmvdMC, [0.99, 0.0036], 1e-10, 0.0)

# Johnson index
johnson = analysis.computeJohnson()
print(f"Computed Johnson = [{johnson[0]}, {johnson[1]}]")
ott.assert_almost_equal(johnson, lmg, 1e-10, 0.0)  # In 2D, Johnson and LMG indices are identical

# VIF metric
vif = analysis.computeVIF()
print(f"Theoretical VIF = [{vif12}, {vif12}]")
print(f"Computed VIF = [{vif[0]}, {vif[1]}]")
ott.assert_almost_equal(vif, [vif12, vif12], 1e-2, 0.0)

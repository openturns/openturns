#! /usr/bin/env python

import openturns as ot
from openturns.testing import assert_almost_equal

ot.TESTPREAMBLE()

print("Fit y ~ 3 - 2 x1 + x2 + epsilon")
kFoldParameter = 4
foldRootSize = 3
# Makes so that k does not divide the sample size.
# In this case, we must take into account for the different weight of
# each fold.
samplingSize = foldRootSize * kFoldParameter + 1
print("samplingSize = ", samplingSize)
aCollection = []
marginal1 = ot.Uniform(-1.0, 1.0)
aCollection.append(marginal1)
aCollection.append(marginal1)
distribution = ot.ComposedDistribution(aCollection)
inputSample = distribution.getSample(samplingSize)
print("inputSample=", inputSample)
g = ot.SymbolicFunction(["x1", "x2"], ["3 - 2 * x1 + x2"])
noise = ot.Normal(0.0, 0.5)
outputSample = g(inputSample) + noise.getSample(samplingSize)
print("outputSample=", outputSample)
lmAlgo = ot.LinearModelAlgorithm(inputSample, outputSample)
result = lmAlgo.getResult()

# Create LOO validation
validationLOO = ot.LinearModelValidation(result, ot.LinearModelValidation.LEAVEONEOUT)
print(validationLOO)
assert validationLOO.getMethod() == ot.LinearModelValidation.LEAVEONEOUT

# Compute analytical LOO MSE
print("Compute Analytical LOO MSE")
mseLOOAnalytical = validationLOO.computeMeanSquaredError()
print("Analytical LOO MSE = ", mseLOOAnalytical)

# Compute naive leave-one-out
residualsLOO = ot.Point(samplingSize)
for j in range(samplingSize):
    indicesLOO = list(range(samplingSize))
    indicesLOO.pop(j)
    inputSampleTrainLOO = inputSample.select(indicesLOO)
    outputSampleTrainLOO = outputSample.select(indicesLOO)
    inputPointLOOTest = inputSample[j]
    outputPointLOOTest = outputSample[j]
    lmAlgoLOO = ot.LinearModelAlgorithm(inputSampleTrainLOO, outputSampleTrainLOO)
    resultLOO = lmAlgoLOO.getResult()
    metamodelLOO = resultLOO.getMetaModel()
    predictionLOOTest = metamodelLOO(inputPointLOOTest)
    residualsLOOTest = predictionLOOTest - outputPointLOOTest
    residualsLOO[j] = residualsLOOTest[0]

mseLOOnaive = residualsLOO.normSquare() / samplingSize
print("Naive LOO MSE = ", mseLOOnaive)

# Test
rtolLOO = 1.0e-12
atolLOO = 0.0
assert_almost_equal(mseLOOAnalytical[0], mseLOOnaive, rtolLOO, atolLOO)

# Check LOO R2
r2ScoreLOO = validationLOO.computeR2Score()
print("Analytical LOO R2 score = ", r2ScoreLOO)
sampleVariance = outputSample.computeCentralMoment(2)
print("sampleVariance = ", sampleVariance)
r2ScoreReference = 1.0 - mseLOOAnalytical[0] / sampleVariance[0]
print("Computed R2 score = ", r2ScoreReference)
rtolLOO = 1.0e-12
atolLOO = 0.0
assert_almost_equal(r2ScoreReference, r2ScoreLOO[0], rtolLOO, atolLOO)

# Create KFold validation
validationKFold = ot.LinearModelValidation(
    result, ot.LinearModelValidation.KFOLD, kFoldParameter
)
print(validationKFold)
assert validationKFold.getKParameter() == kFoldParameter
assert validationKFold.getMethod() == ot.LinearModelValidation.KFOLD

# Compute analytical KFold MSE
mseKFoldAnalytical = validationKFold.computeMeanSquaredError()
print("Analytical KFold MSE=", mseKFoldAnalytical)

# Naive KFold
residualsKFold = ot.Sample(kFoldParameter, 1)
splitter = ot.KFoldSplitter(samplingSize, kFoldParameter)
foldIndex = 0
for indicesTrain, indicesTest in splitter:
    inputSampleKFoldTrain = inputSample[indicesTrain]
    outputSampleKFoldTrain = outputSample[indicesTrain]
    inputSampleKFoldTest = inputSample[indicesTest]
    outputSampleKFoldTest = outputSample[indicesTest]
    lmAlgoKFold = ot.LinearModelAlgorithm(inputSampleKFoldTrain, outputSampleKFoldTrain)
    resultKFold = lmAlgoKFold.getResult()
    metamodelKFold = resultKFold.getMetaModel()
    predictionKFoldTest = metamodelKFold(inputSampleKFoldTest)
    residualsKFoldTest = predictionKFoldTest.asPoint() - outputSampleKFoldTest.asPoint()
    foldSize = indicesTest.getSize()
    residualsKFold[foldIndex, 0] = residualsKFoldTest.normSquare() / foldSize
    foldIndex += 1

mseKFoldnaive = residualsKFold.computeMean()
print("Naive KFold MSE = ", mseKFoldnaive)

# Test
rtolKFold = 1.0e-7
atolKFold = 0.0
assert_almost_equal(mseKFoldAnalytical, mseKFoldnaive, rtolKFold, atolKFold)

# Check K-Fold R2
r2ScoreKFold = validationKFold.computeR2Score()
print("Analytical K-Fold R2 score = ", r2ScoreKFold)
r2ScoreReference = 1.0 - mseKFoldAnalytical[0] / sampleVariance[0]
print("Computed R2 score = ", r2ScoreReference)
rtolKFold = 1.0e-12
atolKFold = 0.0
assert_almost_equal(r2ScoreReference, r2ScoreKFold[0], rtolLOO, atolLOO)

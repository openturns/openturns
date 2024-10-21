#! /usr/bin/env python

import openturns as ot
from openturns.testing import assert_almost_equal
import openturns.experimental as otexp

ot.TESTPREAMBLE()

print("Fit y ~ 3 - 2 x1 + x2 + epsilon")
kFoldParameter = 4
foldRootSize = 3
# Makes so that k does not divide the sample size.
# In this case, we must take the different weigths
# of each fold into account.
sampleSize = foldRootSize * kFoldParameter + 1
print("sampleSize = ", sampleSize)
aCollection = []
marginal1 = ot.Uniform(-1.0, 1.0)
aCollection.append(marginal1)
aCollection.append(marginal1)
distribution = ot.JointDistribution(aCollection)
inputSample = distribution.getSample(sampleSize)
print("inputSample=", inputSample)
g = ot.SymbolicFunction(["x1", "x2"], ["3 - 2 * x1 + x2"])
noise = ot.Normal(0.0, 0.5)
outputSample = g(inputSample) + noise.getSample(sampleSize)
print("outputSample=", outputSample)
lmAlgo = ot.LinearModelAlgorithm(inputSample, outputSample)
result = lmAlgo.getResult()

# Create LOO validation
splitterLOO = ot.LeaveOneOutSplitter(sampleSize)
validationLOO = otexp.LinearModelValidation(result, splitterLOO)
print(validationLOO)

# Compute analytical LOO MSE
print("Compute Analytical LOO MSE")
mseLOOAnalytical = validationLOO.computeMeanSquaredError()
print("Analytical LOO MSE =", mseLOOAnalytical[0])

# Compute naive leave-one-out
residualsLOO = ot.Point(sampleSize)
j = 0
for indicesTrain, indicesTest in splitterLOO:
    inputSampleTrainLOO = inputSample[indicesTrain]
    inputSampleLOOTest = inputSample[indicesTest]
    outputSampleTrainLOO = outputSample[indicesTrain]
    outputSampleLOOTest = outputSample[indicesTest]
    lmAlgoLOO = ot.LinearModelAlgorithm(inputSampleTrainLOO, outputSampleTrainLOO)
    resultLOO = lmAlgoLOO.getResult()
    metamodelLOO = resultLOO.getMetaModel()
    predictionLOOTest = metamodelLOO(inputSampleLOOTest)
    residualsLOOTest = predictionLOOTest.asPoint() - outputSampleLOOTest.asPoint()
    residualsLOO[j] = residualsLOOTest[0]
    j += 1

mseLOOnaive = residualsLOO.normSquare() / sampleSize
print("Naive LOO MSE      =", mseLOOnaive)

# Test
rtolLOO = 1.0e-12
atolLOO = 0.0
assert_almost_equal(mseLOOAnalytical[0], mseLOOnaive, rtolLOO, atolLOO)

# Check LOO R2
r2ScoreLOO = validationLOO.computeR2Score()
print("Analytical LOO R2 score = ", r2ScoreLOO[0])
sampleVariance = outputSample.computeCentralMoment(2)
r2ScoreReference = 1.0 - mseLOOAnalytical[0] / sampleVariance[0]
print("Computed R2 score       = ", r2ScoreReference)
rtolLOO = 1.0e-12
atolLOO = 0.0
assert_almost_equal(r2ScoreReference, r2ScoreLOO[0], rtolLOO, atolLOO)

# Create KFold validation
splitterKFold = ot.KFoldSplitter(sampleSize, kFoldParameter)
validationKFold = otexp.LinearModelValidation(result, splitterKFold)
print(validationKFold)

# Compute analytical KFold MSE
mseKFoldAnalytical = validationKFold.computeMeanSquaredError()
print("Analytical KFold MSE =", mseKFoldAnalytical[0])

# Naive KFold
residualsKFold = ot.Point(sampleSize)
foldIndex = 0
for indicesTrain, indicesTest in splitterKFold:
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
    for k in range(foldSize):
        residualsKFold[indicesTest[k]] = residualsKFoldTest[k]
    foldIndex += 1

mseKFoldnaive = residualsKFold.normSquare() / sampleSize
print("Naive KFold MSE      =", mseKFoldnaive)

# Test
rtolKFold = 1.0e-7
atolKFold = 0.0
assert_almost_equal(mseKFoldAnalytical[0], mseKFoldnaive, rtolKFold, atolKFold)

# Check K-Fold R2
r2ScoreKFold = validationKFold.computeR2Score()
print("Analytical K-Fold R2 score =", r2ScoreKFold[0])
r2ScoreReference = 1.0 - mseKFoldAnalytical[0] / sampleVariance[0]
print("Computed R2 score          =", r2ScoreReference)
rtolKFold = 1.0e-12
atolKFold = 0.0
assert_almost_equal(r2ScoreReference, r2ScoreKFold[0], rtolLOO, atolLOO)

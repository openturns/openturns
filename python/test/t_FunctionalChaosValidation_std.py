#! /usr/bin/env python

import openturns as ot
from openturns.usecases import ishigami_function
from openturns.testing import assert_almost_equal
import openturns.experimental as otexp


def computeMSENaiveLOO(
    inputSample,
    outputSample,
    inputDistribution,
    adaptiveStrategy,
    projectionStrategy,
):
    """
    Compute mean squared error by (naive) LOO.

    Parameters
    ----------
    inputSample : Sample(size, input_dimension)
        The inputSample dataset.
    outputSample : Sample(size, output_dimension)
        The outputSample dataset.
    inputDistribution : ot.Distribution.
        The distribution of the input variable.
    adaptiveStrategy : ot.AdaptiveStrategy
        The method to select relevant coefficients.
    projectionStrategy : ot.ProjectionStrategy
        The method to compute the coefficients.

    Returns
    -------
    mse : Point(output_dimension)
        The mean squared error.
    """
    #
    sampleSize = inputSample.getSize()
    outputDimension = outputSample.getDimension()
    splitter = ot.LeaveOneOutSplitter(sampleSize)
    residualsLOO = ot.Sample(sampleSize, outputDimension)
    indexLOO = 0
    for indicesTrain, indicesTest in splitter:
        inputSampleTrain, inputSampleTest = (
            inputSample[indicesTrain],
            inputSample[indicesTest],
        )
        outputSampleTrain, outputSampleTest = (
            outputSample[indicesTrain],
            outputSample[indicesTest],
        )
        algoLOO = ot.FunctionalChaosAlgorithm(
            inputSampleTrain,
            outputSampleTrain,
            inputDistribution,
            adaptiveStrategy,
            projectionStrategy,
        )
        algoLOO.run()
        chaosResultLOO = algoLOO.getResult()
        metamodelLOO = chaosResultLOO.getMetaModel()
        outputPrediction = metamodelLOO(inputSampleTest)
        for j in range(outputDimension):
            residualsLOO[indexLOO, j] = outputSampleTest[0, j] - outputPrediction[0, j]
        indexLOO += 1
    mse = ot.Point(outputDimension)
    for j in range(outputDimension):
        marginalResidualsLOO = residualsLOO.getMarginal(j).asPoint()
        mse[j] = marginalResidualsLOO.normSquare() / sampleSize
    return mse


def computeMSENaiveKFold(
    inputSample,
    outputSample,
    inputDistribution,
    adaptiveStrategy,
    projectionStrategy,
    kParameter=5,
):
    """
    Compute mean squared error by (naive) KFold.

    Parameters
    ----------
    inputSample : Sample(size, input_dimension)
        The inputSample dataset.
    outputSample : Sample(size, output_dimension)
        The outputSample dataset.
    inputDistribution : ot.Distribution.
        The distribution of the input variable.
    adaptiveStrategy : ot.AdaptiveStrategy
        The method to select relevant coefficients.
    projectionStrategy : ot.ProjectionStrategy
        The method to compute the coefficients.
    kParameter : int, in (2, sampleSize)
        The parameter K.

    Returns
    -------
    mse : Point(output_dimension)
        The mean squared error.
    """
    #
    sampleSize = inputSample.getSize()
    outputDimension = outputSample.getDimension()
    splitter = ot.KFoldSplitter(sampleSize, kParameter)
    squaredResiduals = ot.Sample(sampleSize, outputDimension)
    for indicesTrain, indicesTest in splitter:
        inputSampleTrain, inputSampleTest = (
            inputSample[indicesTrain],
            inputSample[indicesTest],
        )
        outputSampleTrain, outputSampleTest = (
            outputSample[indicesTrain],
            outputSample[indicesTest],
        )
        algoKFold = ot.FunctionalChaosAlgorithm(
            inputSampleTrain,
            outputSampleTrain,
            inputDistribution,
            adaptiveStrategy,
            projectionStrategy,
        )
        algoKFold.run()
        chaosResultKFold = algoKFold.getResult()
        metamodelKFold = chaosResultKFold.getMetaModel()
        predictionsKFold = metamodelKFold(inputSampleTest)
        residualsKFold = outputSampleTest - predictionsKFold
        foldSize = indicesTest.getSize()
        for j in range(outputDimension):
            for i in range(foldSize):
                squaredResiduals[indicesTest[i], j] = residualsKFold[i, j] ** 2
    mse = squaredResiduals.computeMean()
    return mse


ot.TESTPREAMBLE()

# Problem parameters
im = ishigami_function.IshigamiModel()

dimension = im.distributionX.getDimension()

# Compute the sample size from number of folds to guarantee a non constant integer
# number of points per fold
kFoldParameter = 10
foldSampleSize = 20
sampleSize = foldSampleSize * kFoldParameter + 1

degree = 5
enumerateFunction = ot.LinearEnumerateFunction(dimension)
basisSize = enumerateFunction.getBasisSizeFromTotalDegree(degree)
print("basisSize = ", basisSize)
productBasis = ot.OrthogonalProductPolynomialFactory(
    [ot.LegendreFactory()] * dimension, enumerateFunction
)
adaptiveStrategy = ot.FixedStrategy(productBasis, basisSize)
selectionAlgorithm = (
    ot.PenalizedLeastSquaresAlgorithmFactory()
)  # Get a full PCE: do not use model selection.
projectionStrategy = ot.LeastSquaresStrategy(selectionAlgorithm)
inputSample = im.distributionX.getSample(sampleSize)
outputSample = im.model(inputSample)
algo = ot.FunctionalChaosAlgorithm(
    inputSample, outputSample, im.distributionX, adaptiveStrategy, projectionStrategy
)
algo.run()
chaosResult = algo.getResult()

#
print("1. Analytical leave-one-out")
splitterLOO = ot.LeaveOneOutSplitter(sampleSize)
validationLOO = otexp.FunctionalChaosValidation(
    chaosResult, splitterLOO
)
mseLOOAnalytical = validationLOO.computeMeanSquaredError()
print("Analytical LOO MSE = ", mseLOOAnalytical)
assert validationLOO.getSplitter().getN() == sampleSize

# Naive leave-one-out
mseLOOnaive = computeMSENaiveLOO(
    inputSample,
    outputSample,
    im.distributionX,
    adaptiveStrategy,
    projectionStrategy,
)
print("Naive LOO MSE = ", mseLOOnaive)

# Test
rtolLOO = 1.0e-8
atolLOO = 0.0
assert_almost_equal(mseLOOAnalytical, mseLOOnaive, rtolLOO, atolLOO)

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

#
print("2. Analytical K-Fold")
splitterKF = ot.KFoldSplitter(sampleSize, kFoldParameter)
validationKFold = otexp.FunctionalChaosValidation(
    chaosResult, splitterKF
)
print("KFold with K = ", kFoldParameter)
assert validationKFold.getSplitter().getN() == sampleSize

# Compute mean squared error
mseKFoldAnalytical = validationKFold.computeMeanSquaredError()
print("Analytical KFold MSE = ", mseKFoldAnalytical)

# Naive KFold
mseKFoldnaive = computeMSENaiveKFold(
    inputSample,
    outputSample,
    im.distributionX,
    adaptiveStrategy,
    projectionStrategy,
    kFoldParameter,
)
print("Naive KFold MSE = ", mseKFoldnaive)

# Test
rtolKFold = 1.0e-5
atolKFold = 0.0
assert_almost_equal(mseKFoldAnalytical, mseKFoldnaive, rtolKFold, atolKFold)

# Check K-Fold R2
r2ScoreKFold = validationKFold.computeR2Score()
print("Analytical K-Fold R2 score = ", r2ScoreKFold)
r2ScoreReference = 1.0 - mseKFoldAnalytical[0] / sampleVariance[0]
print("Computed R2 score = ", r2ScoreReference)
rtolKFold = 1.0e-12
atolKFold = 0.0
assert_almost_equal(r2ScoreReference, r2ScoreKFold[0], rtolKFold, atolKFold)

#
print("3. Setting FunctionalChaosValidation-ModelSelection to true")
# enables to do LOO CV on a sparse model.
ot.ResourceMap.SetAsBool("FunctionalChaosValidation-ModelSelection", True)
selectionAlgorithm = (
    ot.LeastSquaresMetaModelSelectionFactory()
)  # Get a sparse PCE (i.e. with model selection).
projectionStrategy = ot.LeastSquaresStrategy(selectionAlgorithm)
inputSample = im.distributionX.getSample(sampleSize)
outputSample = im.model(inputSample)
algo = ot.FunctionalChaosAlgorithm(
    inputSample, outputSample, im.distributionX, adaptiveStrategy, projectionStrategy
)
algo.run()
chaosResult = algo.getResult()

# Analytical leave-one-out
splitterLOO = ot.LeaveOneOutSplitter(sampleSize)
validationLOO = otexp.FunctionalChaosValidation(
    chaosResult, splitterLOO
)
mseLOOAnalytical = validationLOO.computeMeanSquaredError()
print("Analytical LOO MSE = ", mseLOOAnalytical)
# Naive leave-one-out
mseLOOnaive = computeMSENaiveLOO(
    inputSample,
    outputSample,
    im.distributionX,
    adaptiveStrategy,
    projectionStrategy,
)
print("Naive LOO MSE = ", mseLOOnaive)
# Test
rtolLOO = 1.0e-1  # We cannot have more accuracy, as the MSE estimator is then biased
atolLOO = 0.0
assert_almost_equal(mseLOOAnalytical, mseLOOnaive, rtolLOO, atolLOO)

#! /usr/bin/env python

import openturns as ot
from openturns.usecases import ishigami_function
from openturns.testing import assert_almost_equal, assert_raises


def computeMSENaiveLOO(
    inputSample,
    outputSample,
    distribution,
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
    distribution : ot.Distribution.
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
            distribution,
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
    distribution,
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
    distribution : ot.Distribution.
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
            distribution,
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

dimension = im.distribution.getDimension()

# Compute the sample size from number of folds to guarantee a non constant integer
# number of points per fold
kFoldParameter = 10
foldSampleSize = 20
sampleSize = foldSampleSize * kFoldParameter + 1

# Degree 4 keeps the design comfortably overdetermined (35 coefficients
# for 201 points) while making the 400+ naive refits cheaper
degree = 4
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
inputSample = im.distribution.getSample(sampleSize)
outputSample = im.model(inputSample)
algo = ot.FunctionalChaosAlgorithm(
    inputSample,
    outputSample,
    im.distribution,
    adaptiveStrategy,
    projectionStrategy,
)
algo.run()
chaosResult = algo.getResult()

#
print("1. Analytical leave-one-out")
splitterLOO = ot.LeaveOneOutSplitter(sampleSize)
validationLOO = ot.FunctionalChaosValidation(chaosResult, splitterLOO)
mseLOOAnalytical = validationLOO.computeMeanSquaredError()
print("Analytical LOO MSE = ", mseLOOAnalytical)
assert validationLOO.getSplitter().getN() == sampleSize

# Naive leave-one-out
mseLOOnaive = computeMSENaiveLOO(
    inputSample,
    outputSample,
    im.distribution,
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
validationKFold = ot.FunctionalChaosValidation(chaosResult, splitterKF)
print("KFold with K = ", kFoldParameter)
assert validationKFold.getSplitter().getN() == sampleSize

# Compute mean squared error
mseKFoldAnalytical = validationKFold.computeMeanSquaredError()
print("Analytical KFold MSE = ", mseKFoldAnalytical)

# Naive KFold
mseKFoldnaive = computeMSENaiveKFold(
    inputSample,
    outputSample,
    im.distribution,
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
inputSample = im.distribution.getSample(sampleSize)
outputSample = im.model(inputSample)
algo = ot.FunctionalChaosAlgorithm(
    inputSample,
    outputSample,
    im.distribution,
    adaptiveStrategy,
    projectionStrategy,
)
algo.run()
chaosResult = algo.getResult()

# Analytical leave-one-out
splitterLOO = ot.LeaveOneOutSplitter(sampleSize)
validationLOO = ot.FunctionalChaosValidation(chaosResult, splitterLOO)
mseLOOAnalytical = validationLOO.computeMeanSquaredError()
print("Analytical LOO MSE = ", mseLOOAnalytical)
# Naive leave-one-out
mseLOOnaive = computeMSENaiveLOO(
    inputSample,
    outputSample,
    im.distribution,
    adaptiveStrategy,
    projectionStrategy,
)
print("Naive LOO MSE = ", mseLOOnaive)
# Test
rtolLOO = 1.0e-1  # We cannot have more accuracy, as the MSE estimator is then biased
atolLOO = 0.0
assert_almost_equal(mseLOOAnalytical, mseLOOnaive, rtolLOO, atolLOO)

# Weighted validation: the scores of a derived validation class follow the
# weights given to setWeights, and coincide with the weighted recomputation
# of the mean squared error
validationWeighted = ot.FunctionalChaosValidation(chaosResult, splitterLOO)
validationWeighted.setWeights(
    [1.0 + 0.5 * (i % 3) for i in range(chaosResult.getOutputSample().getSize())]
)
residual = validationWeighted.getResidualSample()
predictions = validationWeighted.getMetamodelPredictions()
weights = validationWeighted.getWeights()
weightSum = sum(weights)
expectedMSE = (
    sum(
        w * (residual[i, 0]) ** 2
        for i, w in enumerate(weights)
    )
    / weightSum
)
assert_almost_equal(
    validationWeighted.computeMeanSquaredError()[0], expectedMSE, 1.0e-12, 1.0e-12
)
# A uniform weight equal to one reproduces the unweighted score
validationUniform = ot.FunctionalChaosValidation(chaosResult, splitterLOO)
validationUniform.setWeights([2.0] * chaosResult.getOutputSample().getSize())
assert_almost_equal(
    validationUniform.computeMeanSquaredError(),
    validationLOO.computeMeanSquaredError(),
    1.0e-12,
    1.0e-12,
)

#
print("4. Weighted design: analytical LOO and KFold are exact")
weightedDegree = 3
weightedBasisSize = enumerateFunction.getBasisSizeFromTotalDegree(weightedDegree)
weightedAdaptive = ot.FixedStrategy(productBasis, weightedBasisSize)
weightedSelection = ot.PenalizedLeastSquaresAlgorithmFactory()
weightedProjection = ot.LeastSquaresStrategy(weightedSelection)
weightedSize = 60
weightedInput = im.distribution.getSample(weightedSize)
weightedOutput = im.model(weightedInput)
# Non-uniform design weights, as produced by a quadrature rule
designWeights = [0.5 + 0.25 * (i % 5) for i in range(weightedSize)]
weightedAlgo = ot.FunctionalChaosAlgorithm(
    weightedInput,
    designWeights,
    weightedOutput,
    im.distribution,
    weightedAdaptive,
    weightedProjection,
)
weightedAlgo.run()
weightedResult = weightedAlgo.getResult()
# The result carries the design weights
assert_almost_equal(
    weightedResult.getWeights(), ot.Point(designWeights), 1.0e-12, 1.0e-12
)

# Brute-force weighted LOO: refit without each point
weightedSplitterLOO = ot.LeaveOneOutSplitter(weightedSize)
weightedValidationLOO = ot.FunctionalChaosValidation(
    weightedResult, weightedSplitterLOO
)
press = 0.0
weightSum = sum(designWeights)
for i in range(weightedSize):
    train = ot.Indices([j for j in range(weightedSize) if j != i])
    algoRefit = ot.FunctionalChaosAlgorithm(
        weightedInput[train],
        [designWeights[j] for j in train],
        weightedOutput[train],
        im.distribution,
        weightedAdaptive,
        ot.LeastSquaresStrategy(weightedSelection),
    )
    algoRefit.run()
    prediction = algoRefit.getResult().getMetaModel()(weightedInput[i])
    press += designWeights[i] * (weightedOutput[i, 0] - prediction[0]) ** 2
press /= weightSum
assert_almost_equal(
    weightedValidationLOO.computeMeanSquaredError()[0], press, 1.0e-10, 1.0e-12
)

# Brute-force weighted KFold: refit without each fold
kWeighted = 5
weightedSplitterKF = ot.KFoldSplitter(weightedSize, kWeighted)
weightedValidationKF = ot.FunctionalChaosValidation(
    weightedResult, weightedSplitterKF
)
kfoldError = 0.0
for indicesTrain, indicesTest in weightedSplitterKF:
    algoRefit = ot.FunctionalChaosAlgorithm(
        weightedInput[indicesTrain],
        [designWeights[j] for j in indicesTrain],
        weightedOutput[indicesTrain],
        im.distribution,
        weightedAdaptive,
        ot.LeastSquaresStrategy(weightedSelection),
    )
    algoRefit.run()
    predictions = algoRefit.getResult().getMetaModel()(
        weightedInput[indicesTest]
    )
    for t in range(indicesTest.getSize()):
        kfoldError += (
            designWeights[indicesTest[t]]
            * (weightedOutput[indicesTest[t], 0] - predictions[t, 0]) ** 2
        )
kfoldError /= weightSum
assert_almost_equal(
    weightedValidationKF.computeMeanSquaredError()[0], kfoldError, 1.0e-10, 1.0e-12
)

# Weighted R2 against the weighted variance
weightedMean = (
    sum(designWeights[i] * weightedOutput[i, 0] for i in range(weightedSize))
    / weightSum
)
weightedVariance = (
    sum(
        designWeights[i] * (weightedOutput[i, 0] - weightedMean) ** 2
        for i in range(weightedSize)
    )
    / weightSum
)
assert_almost_equal(
    weightedValidationLOO.computeR2Score()[0],
    1.0 - press / weightedVariance,
    1.0e-10,
    1.0e-12,
)

# CorrectedLeaveOneOut rejects the weighted design, LeaveOneOut accepts it
weightedBasis = ot.Basis([productBasis.build(i) for i in range(weightedBasisSize)])
proxyW = ot.DesignProxy(weightedInput, weightedBasis)
methodW = ot.QRMethod(proxyW, ot.Point(designWeights), list(range(weightedBasisSize)))
with assert_raises(TypeError):
    ot.CorrectedLeaveOneOut().run(methodW, weightedOutput.getMarginal(0))
scoreW = ot.LeaveOneOut().run(methodW, weightedOutput.getMarginal(0))
assert scoreW >= 0.0

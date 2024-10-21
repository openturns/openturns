#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
from openturns.usecases import ishigami_function

ot.TESTPREAMBLE()

# Create Ishigami
im = ishigami_function.IshigamiModel()
input_names = im.inputDistribution.getDescription()
sampleSize = 500
inputSample = im.inputDistribution.getSample(sampleSize)
outputSample = im.model(inputSample)

# Create PCE
multivariateBasis = ot.OrthogonalProductPolynomialFactory([im.X1, im.X2, im.X3])
totalDegree = 12
enumerateFunction = multivariateBasis.getEnumerateFunction()
basisSize = enumerateFunction.getBasisSizeFromTotalDegree(totalDegree)
selectionAlgorithm = ot.LeastSquaresMetaModelSelectionFactory()
projectionStrategy = ot.LeastSquaresStrategy(
    inputSample, outputSample, selectionAlgorithm
)
adaptiveStrategy = ot.FixedStrategy(multivariateBasis, basisSize)
chaosAlgorithm = ot.FunctionalChaosAlgorithm(
    inputSample,
    outputSample,
    im.inputDistribution,
    adaptiveStrategy,
    projectionStrategy,
)
chaosAlgorithm.run()
chaosResult = chaosAlgorithm.getResult()

# Create list of functions and the corresponding marginal indices
listOfParametricFunctions = []
listOfConditioningIndices = []
# Given X1
conditionalExpectationGivenX1 = ot.SymbolicFunction(
    ["a", "b", "x1"], ["a / 2 + (1 + b * pi_^4 / 5) * sin(x1)"]
)
listOfParametricFunctions.append(conditionalExpectationGivenX1)
listOfConditioningIndices.append([0])
# Given X2
conditionalExpectationGivenX2 = ot.SymbolicFunction(["a", "b", "x2"], ["a * sin(x2)^2"])
listOfParametricFunctions.append(conditionalExpectationGivenX2)
listOfConditioningIndices.append([1])
# Given X3
conditionalExpectationGivenX3 = ot.SymbolicFunction(["a", "b", "x3"], ["a / 2"])
listOfParametricFunctions.append(conditionalExpectationGivenX3)
listOfConditioningIndices.append([2])
# Given X1, X2
conditionalExpectationGivenX1X2 = ot.SymbolicFunction(
    ["a", "b", "x1", "x2"], ["a * sin(x2)^2 + (1 + b * pi_^4 / 5) * sin(x1)"]
)
listOfParametricFunctions.append(conditionalExpectationGivenX1X2)
listOfConditioningIndices.append([0, 1])
# Given X1, X3
conditionalExpectationGivenX1X3 = ot.SymbolicFunction(
    ["a", "b", "x1", "x3"], ["a / 2 + (1 + b * x3^4) * sin(x1)"]
)
listOfParametricFunctions.append(conditionalExpectationGivenX1X3)
listOfConditioningIndices.append([0, 2])
# Given X2, X3
conditionalExpectationGivenX2X3 = ot.SymbolicFunction(
    ["a", "b", "x2", "x3"], ["a * sin(x2)^2"]
)
listOfParametricFunctions.append(conditionalExpectationGivenX2X3)
listOfConditioningIndices.append([1, 2])
# Given X1, X2, X3
conditionalExpectationGivenX1X2X3 = ot.SymbolicFunction(
    ["a", "b", "x1", "x2", "x3"], ["sin(x1) + a * (sin(x2)) ^ 2 + b * x3^4 * sin(x1)"]
)
listOfParametricFunctions.append(conditionalExpectationGivenX1X2X3)
listOfConditioningIndices.append([0, 1, 2])
#
sampleSizeTest = 10000
absoluteTolerance = 1.0e-3
print("absoluteTolerance = ", absoluteTolerance)
for index in range(len(listOfParametricFunctions)):
    conditionalExpectationParametric = listOfParametricFunctions[index]
    conditioningIndices = listOfConditioningIndices[index]
    # PCE | Xi
    conditionalPCE = chaosResult.getConditionalExpectation(conditioningIndices)
    conditionalPCEFunction = conditionalPCE.getMetaModel()
    # Exact conditional expectation
    conditionalExpectationFunctionExact = ot.ParametricFunction(
        conditionalExpectationParametric, [0, 1], [im.a, im.b]
    )
    marginalDistribution = im.inputDistribution.getMarginal(conditioningIndices)
    # Compute L2 error between the two functions
    experiment = ot.LowDiscrepancyExperiment(
        ot.SobolSequence(), marginalDistribution, sampleSizeTest, True
    )
    # experiment = ot.MonteCarloExperiment(marginalDistribution, sampleSizeTest)
    integration = otexp.ExperimentIntegration(experiment)
    error = integration.computeL2Norm(
        conditionalPCEFunction - conditionalExpectationFunctionExact
    )
    print(f"PCE | X{conditioningIndices}, L2 error = ", error[0])
    assert error[0] < absoluteTolerance
